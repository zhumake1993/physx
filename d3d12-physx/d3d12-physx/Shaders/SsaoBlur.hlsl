//=============================================================================
// �Ի�����ͼʹ�ñ߽籣��ģ���㷨
// ʹ��������ɫ�����������ɫ�����Ա���Ӽ���ģʽת������Ⱦģʽ
// ����cache���Բ����ֲ�û��ʹ�ù����ڴ����ʧ
// ������ͼʹ��16λ�����ʽ������С��ʹ���ܹ���cache�д洢���ͼԪ
//=============================================================================

cbuffer cbSsao : register(b0)
{
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gProjTex;
    float4   gOffsetVectors[14];

    float4 gBlurWeights[3];
    float2 gInvRenderTargetSize;

    float gOcclusionRadius;
    float gOcclusionFadeStart;
    float gOcclusionFadeEnd;
    float gSurfaceEpsilon;
};

cbuffer cbRootConstants : register(b1)
{
    bool gHorizontalBlur;
};

Texture2D gNormalMap : register(t0);
Texture2D gDepthMap  : register(t1);
Texture2D gInputMap  : register(t2);
 
SamplerState gsamPointClamp : register(s0);
SamplerState gsamLinearClamp : register(s1);
SamplerState gsamDepthMap : register(s2);
SamplerState gsamLinearWrap : register(s3);

static const int gBlurRadius = 5;
 
static const float2 gTexCoords[6] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};
 
struct VertexOut
{
    float4 PosH  : SV_POSITION;
	float2 TexC  : TEXCOORD;
};

VertexOut VS(uint vid : SV_VertexID)
{
    VertexOut vout;

    vout.TexC = gTexCoords[vid];

    // NDC�ռ��и�����Ļ��quad
    vout.PosH = float4(2.0f*vout.TexC.x - 1.0f, 1.0f - 2.0f*vout.TexC.y, 0.0f, 1.0f);

    return vout;
}

float NdcDepthToViewDepth(float z_ndc)
{
    // z_ndc = A + B/viewZ��gProj[2,2]=A��gProj[3,2]=B.
    float viewZ = gProj[3][2] / (z_ndc - gProj[2][2]);
    return viewZ;
}

float4 PS(VertexOut pin) : SV_Target
{
    // �������������
    float blurWeights[12] =
    {
        gBlurWeights[0].x, gBlurWeights[0].y, gBlurWeights[0].z, gBlurWeights[0].w,
        gBlurWeights[1].x, gBlurWeights[1].y, gBlurWeights[1].z, gBlurWeights[1].w,
        gBlurWeights[2].x, gBlurWeights[2].y, gBlurWeights[2].z, gBlurWeights[2].w,
    };

	float2 texOffset;
	if(gHorizontalBlur)
	{
		texOffset = float2(gInvRenderTargetSize.x, 0.0f);
	}
	else
	{
		texOffset = float2(0.0f, gInvRenderTargetSize.y);
	}

	// �ȼ����ĵ��ֵ
	float4 color      = blurWeights[gBlurRadius] * gInputMap.SampleLevel(gsamPointClamp, pin.TexC, 0.0);
	float totalWeight = blurWeights[gBlurRadius];
	 
    float3 centerNormal = gNormalMap.SampleLevel(gsamPointClamp, pin.TexC, 0.0f).xyz;
    float  centerDepth = NdcDepthToViewDepth(
        gDepthMap.SampleLevel(gsamDepthMap, pin.TexC, 0.0f).r);

	for(float i = -gBlurRadius; i <=gBlurRadius; ++i)
	{
		// ���ĵ��ֵ�Ѿ��ӹ���
		if( i == 0 )
			continue;

		float2 tex = pin.TexC + i*texOffset;

		float3 neighborNormal = gNormalMap.SampleLevel(gsamPointClamp, tex, 0.0f).xyz;
        float  neighborDepth  = NdcDepthToViewDepth(
            gDepthMap.SampleLevel(gsamDepthMap, tex, 0.0f).r);

		// ������ĵ��ֵ�������������ֵ�����ٽ����ֵ������
		// ��ٶ���������˱߽磬�������ò���
	
		if( dot(neighborNormal, centerNormal) >= 0.8f &&
		    abs(neighborDepth - centerDepth) <= 0.2f )
		{
            float weight = blurWeights[i + gBlurRadius];

			color += weight*gInputMap.SampleLevel(
                gsamPointClamp, tex, 0.0);
		
			totalWeight += weight;
		}
	}

	// ����������һЩ�����㣬����Ȩ���ܺͲ�Ϊ1����Ҫ����
    return color / totalWeight;
}
