cbuffer cbSettings : register(b0)
{
	// 常量缓冲中不能有数组项，因此需要列出每个元素

	// 是否有纹理
	float hasTex;

	// 左上角在NDC空间中的坐标
	float x;
	float y;
	float z;

	// 宽高
	float w;
	float h;

	// 颜色
	float r;
	float g;
	float b;
	float a;
};

Texture2D gMap : register(t0);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float2 TexC    : TEXCOORD;
};

VertexOut VS(uint vid : SV_VertexID)
{
	VertexOut vout = (VertexOut)0.0f;

	float2 gTexCoords[6] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(1.0f, 0.0f),
		float2(1.0f, 1.0f)
	};

	float4 gPosCoords[6] =
	{
		float4(x, y - h, z, 1.0f),
		float4(x, y, z, 1.0f),
		float4(x + w, y, z, 1.0f),
		float4(x, y - h, z, 1.0f),
		float4(x + w, y, z, 1.0f),
		float4(x + w, y - h, z, 1.0f),
	};
	
	vout.PosH = gPosCoords[vid];

	vout.TexC = gTexCoords[vid];

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	if (hasTex == 1.0f)
		return float4(r, g, b, a) * gMap.SampleLevel(gsamLinearWrap, pin.TexC, 0.0f);
	else
		return float4(r, g, b, a);
}


