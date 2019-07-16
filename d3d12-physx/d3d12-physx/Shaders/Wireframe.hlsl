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
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	VertexOut vout = (VertexOut)0.0f;

	// ��ȡʵ������
	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;

    // �任������ռ�
    float4 posW = mul(float4(vin.PosL, 1.0f), world);

    // �任����μ��ÿռ�
    vout.PosH = mul(posW, gViewProj);

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return float4(1.0f,1.0f,1.0f,1.0f);
}