cbuffer cbSsao : register(b0)
{
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gProjTex;
	float4   gOffsetVectors[14];

	float4 gBlurWeights[3];
	float2 gInvRenderTargetSize;

    // �ڱβ���
    float    gOcclusionRadius;
    float    gOcclusionFadeStart;
    float    gOcclusionFadeEnd;
    float    gSurfaceEpsilon;
};
 
cbuffer cbRootConstants : register(b1)
{
	bool gHorizontalBlur;
};

Texture2D gNormalMap    : register(t0);
Texture2D gDepthMap     : register(t1);
Texture2D gRandomVecMap : register(t2);

SamplerState gsamPointClamp : register(s0);
SamplerState gsamLinearClamp : register(s1);
SamplerState gsamDepthMap : register(s2);
SamplerState gsamLinearWrap : register(s3);

static const int gSampleCount = 14;
 
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
    float4 PosH : SV_POSITION;
    float3 PosV : POSITION;
	float2 TexC : TEXCOORD0;
};

VertexOut VS(uint vid : SV_VertexID)
{
    VertexOut vout;

    vout.TexC = gTexCoords[vid];

    // ������Ļ��quad��NDC�ռ��е�����
    vout.PosH = float4(2.0f*vout.TexC.x - 1.0f, 1.0f - 2.0f*vout.TexC.y, 0.0f, 1.0f);
 
    // ��quad�任���ӿռ��ƽ��
    float4 ph = mul(vout.PosH, gInvProj);
    vout.PosV = ph.xyz / ph.w;

    return vout;
}

// ����r�ڱ�p�ĳ̶�
float OcclusionFunction(float distZ)
{
	// ���r��p�ĺ��棬��r�޷��ڱ�p
	// ���distZ�㹻С����Ҳ��Ϊr�޷��ڱ�p
	// �ڱγ̶ȵļ��㺯������
	//       1.0     -------------\
	//               |           |  \
	//               |           |    \
	//               |           |      \ 
	//               |           |        \
	//               |           |          \
	//               |           |            \
	//  ------|------|-----------|-------------|---------|--> zv
	//        0     Eps          z0            z1        

	float occlusion = 0.0f;
	if(distZ > gSurfaceEpsilon)
	{
		float fadeLength = gOcclusionFadeEnd - gOcclusionFadeStart;
		
		// distZ��gOcclusionFadeStart������gOcclusionFadeEndʱ���ڱ����Լ���
		occlusion = saturate( (gOcclusionFadeEnd-distZ)/fadeLength );
	}
	
	return occlusion;	
}

float NdcDepthToViewDepth(float z_ndc)
{
    // z_ndc = A + B/viewZ, gProj[2,2]=A��gProj[3,2]=B.
    float viewZ = gProj[3][2] / (z_ndc - gProj[2][2]);
    return viewZ;
}
 
float4 PS(VertexOut pin) : SV_Target
{
	// p -- ���㻷���ڱεĵ�
	// n -- p��ķ�����
	// q -- ʼ��p�����ƫ������
	// r -- Ǳ���ڱε�

	// ��ȡ�����ص��ӿռ䷨������z����
	float3 n = normalize(gNormalMap.SampleLevel(gsamPointClamp, pin.TexC, 0.0f).xyz);
    float pz = gDepthMap.SampleLevel(gsamDepthMap, pin.TexC, 0.0f).r;
	pz = NdcDepthToViewDepth(pz);

	// �ع�p���������ӿռ�λ��(x,y,z)
	// �ҵ�tʹ��p = t * pin.PosV
	// p.z = t * pin.PosV.z
	// t = p.z / pin.PosV.z
	float3 p = (pz/pin.PosV.z)*pin.PosV;
	
	// �����漴������ӳ��[0,1] --> [-1, +1].
	float3 randVec = 2.0f * gRandomVecMap.SampleLevel(gsamLinearWrap, 4.0f * pin.TexC, 0.0f).rgb - 1.0f;

	float occlusionSum = 0.0f;
	
	// ��p����������nָ���������ٽ���
	for(int i = 0; i < gSampleCount; ++i)
	{
		// ���е�ƫ���������Ǿ��ȷֲ��ģ���ͬʱҲ�ǳ���̶���
		// ������ƫ����������һ�����������ת
		// �Ӷ��õ���������ľ��ȷֲ�����
		float3 offset = reflect(gOffsetVectors[i].xyz, randVec);
	
		// ���������(p, n)�����ƽ��ı��棬��ȡ��
		float flip = sign( dot(offset, n) );
		
		// ���ڱΰ뾶�ڲ���һ����q
		float3 q = p + flip * gOcclusionRadius * offset;
		
		// ����q��ͶӰ��������
		float4 projQ = mul(float4(q, 1.0f), gProjTex);
		projQ /= projQ.w;

		// �ҵ��ش��۾���q�����ߵ�������ֵ������q�����ֵ��
		// ��Ҫ�������ͼ�в���
		float rz = gDepthMap.SampleLevel(gsamDepthMap, projQ.xy, 0.0f).r;
        rz = NdcDepthToViewDepth(rz);

		// �ع�r���������ӿռ�λ��(rx,ry,rz)
		// ����tʹ��r = t*q
		// r.z = t*q.z ==> t = r.z / q.z
		float3 r = (rz / q.z) * q;
		
		// ����r�Ƿ��ڱ�p
		//   * dot(n, normalize(r - p))�����ڱε�r��ƽ��(p,n)ǰ��ĳ̶�
		//     ��ֵԽ�����Ǹ���Խ����ڱ�Ȩ��
		//     ��Ҳ���Է�ֹr��p��ͬһƽ������
		//   * ����p��r֮��ľ�������ڱ�Ȩ��
		//     ���r����p��Զ����r�޷��ڱ�p
		float distZ = p.z - r.z;
		float dp = max(dot(n, normalize(r - p)), 0.0f);

        float occlusion = dp*OcclusionFunction(distZ);

		occlusionSum += occlusion;
	}
	
	occlusionSum /= gSampleCount;
	
	float access = 1.0f - occlusionSum;

	// ����ssao��ͼ�ĶԱȶȣ����ssao��Ч��������
	return saturate(pow(access, 6.0f));
}
