#include "Common.hlsl"
 
struct VertexIn
{
	float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentU : TANGENT;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float4 ShadowPosH : POSITION0;
	float4 SsaoPosH   : POSITION1;
    float3 PosW    : POSITION2;
    float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 TexC    : TEXCOORD;

	// nointerpolation使得索引不会被插值
	nointerpolation uint MatIndex  : MATINDEX;
	nointerpolation uint ReceiveShadow : RECEIVESHADOW;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	VertexOut vout = (VertexOut)0.0f;

	// 获取实例数据
	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;
	float4x4 invTraWorld = instData.InvTraWorld;
	float4x4 texTransform = instData.TexTransform;
	uint matIndex = instData.MaterialIndex;
	uint receiveShadow = instData.ReceiveShadow;

	vout.MatIndex = matIndex;
	vout.ReceiveShadow = receiveShadow;

	// 获取材质数据
	MaterialData matData = gMaterialData[matIndex];
	
    // 变换到世界空间
    float4 posW = mul(float4(vin.PosL, 1.0f), world);
    vout.PosW = posW.xyz;

	// 假定世界矩阵是正交的，否则需要计算逆转置矩阵
	// 这里直接使用逆转置矩阵
    vout.NormalW = mul(vin.NormalL, (float3x3)invTraWorld);
	vout.TangentW = mul(vin.TangentU, (float3x3)invTraWorld);

    // 变换到齐次剪裁空间
    vout.PosH = mul(posW, gViewProj);

	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), texTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;

	// 计算投影纹理坐标
	vout.ShadowPosH = mul(posW, gShadowTransform);

	// 产生投影纹理坐标，将ssao贴图映射至屏幕
	vout.SsaoPosH = mul(posW, gViewProjTex);

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// 获取材质数据
	MaterialData matData = gMaterialData[pin.MatIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	float3 fresnelR0 = matData.FresnelR0;
	float  roughness = matData.Roughness;
	uint diffuseMapIndex = matData.DiffuseMapIndex;
	uint normalMapIndex = matData.NormalMapIndex;

	// 在数组中动态查找纹理
	diffuseAlbedo *= gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);

#ifdef ALPHA_TEST
	// 丢弃纹理alpha值小于0.1的像素
	// 在着色器中尽早做该测试，以便于更早地离开着色器，从而省去后续的计算
	clip(diffuseAlbedo.a - 0.1f);
#endif

	// 插值法向量会造成非单位法向量，因此需要规整
	pin.NormalW = normalize(pin.NormalW);

	float4 normalMapSample = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float3 bumpedNormalW = pin.NormalW;

	if (normalMapIndex != -1) {
		normalMapSample = gTextureMaps[normalMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
		bumpedNormalW = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);
	}

	float3 toEyeW = gEyePosW - pin.PosW;
	float distToEye = length(toEyeW);
	toEyeW /= distToEye; // 单位化

	// 完成纹理投影并采样ssao贴图
	pin.SsaoPosH /= pin.SsaoPosH.w;
	float ambientAccess = gSsaoMap.Sample(gsamLinearClamp, pin.SsaoPosH.xy, 0.0f).r;

	// 环境光
    float4 ambient = ambientAccess * gAmbientLight* diffuseAlbedo;

	// 只有第一个光源产生阴影
	float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);

	if (pin.ReceiveShadow == 1) {
		shadowFactor[0] = CalcShadowFactor(pin.ShadowPosH);
	}

    const float shininess = (1.0f - roughness) * normalMapSample.a;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
		bumpedNormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

	// 镜面反射
	float3 r = reflect(-toEyeW, bumpedNormalW);
	r = BoxCubeMapLookup(pin.PosW, normalize(r), float3(0.0f, 0.0f, 0.0f), float3(2500.0f, 2500.0f, 2500.0f));
	r = normalize(r); // 单位化
	float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
	float3 fresnelFactor = SchlickFresnel(fresnelR0, bumpedNormalW, r);
	litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;

#ifdef FOG
	// 计算雾
	float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
	litColor = lerp(litColor, gFogColor, fogAmount);
#endif

    // 通常从漫反射材质中提取alpha值
    litColor.a = diffuseAlbedo.a;

    return litColor;
}