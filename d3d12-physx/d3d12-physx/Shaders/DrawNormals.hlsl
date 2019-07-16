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
	float4 PosH     : SV_POSITION;
    float3 NormalW  : NORMAL;
	float3 TangentW : TANGENT;
	float2 TexC     : TEXCOORD;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	VertexOut vout = (VertexOut)0.0f;

	// 获取实例数据
	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;
	float4x4 invTraWorld = instData.InvTraWorld;

	// 变换到世界空间
	float4 posW = mul(float4(vin.PosL, 1.0f), world);

	// 假定世界矩阵是正交的，否则需要计算逆转置矩阵
	// 这里直接使用逆转置矩阵
	vout.NormalW = mul(vin.NormalL, (float3x3)invTraWorld);
	vout.TangentW = mul(vin.TangentU, (float3x3)invTraWorld);

	// 变换到齐次剪裁空间
	vout.PosH = mul(posW, gViewProj);

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// 插值法向量会造成非单位法向量，因此需要规整
    pin.NormalW = normalize(pin.NormalW);
	
    // 使用插值的顶点法向量来进行SSAO
    // 写入法向量在视空间中的坐标
    float3 normalV = mul(pin.NormalW, (float3x3)gView);
    return float4(normalV, 0.0f);
}


