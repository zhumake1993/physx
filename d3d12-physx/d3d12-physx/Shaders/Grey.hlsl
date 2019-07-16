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
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 TexC    : TEXCOORD;
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

	// 获取材质数据
	MaterialData matData = gMaterialData[matIndex];
	
    // 变换到世界空间
    float4 posW = mul(float4(vin.PosL, 1.0f), world);
    vout.PosW = posW.xyz;

	// 假定世界矩阵是正交的，否则需要计算逆转置矩阵
	// 这里直接使用逆转置矩阵
    vout.NormalW = mul(vin.NormalL, (float3x3)invTraWorld);
	vout.TangentW = mul(vin.TangentU, (float3x3)invTraWorld);

    // 变换到其次剪裁空间
    vout.PosH = mul(posW, gViewProj);

	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), texTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return float4(0.1f, 0.1f, 0.1f, 1.0f);
}


