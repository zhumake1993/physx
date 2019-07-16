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

	// ��ȡʵ������
	InstanceData instData = gInstanceData[instanceID];
	float4x4 world = instData.World;
	float4x4 invTraWorld = instData.InvTraWorld;

	// �任������ռ�
	float4 posW = mul(float4(vin.PosL, 1.0f), world);

	// �ٶ���������������ģ�������Ҫ������ת�þ���
	// ����ֱ��ʹ����ת�þ���
	vout.NormalW = mul(vin.NormalL, (float3x3)invTraWorld);
	vout.TangentW = mul(vin.TangentU, (float3x3)invTraWorld);

	// �任����μ��ÿռ�
	vout.PosH = mul(posW, gViewProj);

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// ��ֵ����������ɷǵ�λ�������������Ҫ����
    pin.NormalW = normalize(pin.NormalW);
	
    // ʹ�ò�ֵ�Ķ��㷨����������SSAO
    // д�뷨�������ӿռ��е�����
    float3 normalV = mul(pin.NormalW, (float3x3)gView);
    return float4(normalV, 0.0f);
}


