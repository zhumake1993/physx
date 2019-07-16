cbuffer cbSettings : register(b0)
{
	// ���������в���������������Ҫ�г�ÿ��Ԫ��
	int gBlurRadius;

	// 11��ģ��Ȩ��
	float w0;
	float w1;
	float w2;
	float w3;
	float w4;
	float w5;
	float w6;
	float w7;
	float w8;
	float w9;
	float w10;
};

static const int gMaxBlurRadius = 5;


Texture2D gInput            : register(t0);
RWTexture2D<float4> gOutput : register(u0);

#define N 256
#define CacheSize (N + 2*gMaxBlurRadius)
groupshared float4 gCache[CacheSize];

[numthreads(N, 1, 1)]
void HorzBlurCS(int3 groupThreadID : SV_GroupThreadID,
				int3 dispatchThreadID : SV_DispatchThreadID)
{
	float weights[11] = { w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10 };

	// ʹ�ù����ڴ������ٴ���
	// ģ��N�����أ���Ҫ����N + 2*BlurRadius������
	
	// ���߳�������N���߳�
	// Ϊ�˻�ö����2*BlurRadius�����أ���Ҫ2*BlurRadius���߳����������������
	if(groupThreadID.x < gBlurRadius) // ����ߵ�gBlurRadius���߳�
	{
		// ����ͼ���Ե
		int x = max(dispatchThreadID.x - gBlurRadius, 0);
		gCache[groupThreadID.x] = gInput[int2(x, dispatchThreadID.y)];
	}
	if(groupThreadID.x >= N-gBlurRadius) // ���ұߵ�gBlurRadius���߳�
	{
		// ����ͼ���Ե
		int x = min(dispatchThreadID.x + gBlurRadius, gInput.Length.x-1);
		gCache[groupThreadID.x+2*gBlurRadius] = gInput[int2(x, dispatchThreadID.y)];
	}

	gCache[groupThreadID.x+gBlurRadius] = gInput[min(dispatchThreadID.xy, gInput.Length.xy-1)];

	// �ȴ������߳����
	GroupMemoryBarrierWithGroupSync();
	
	//
	// ģ������
	//

	float4 blurColor = float4(0, 0, 0, 0);
	
	for(int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.x + gBlurRadius + i;
		
		blurColor += weights[i+gBlurRadius]*gCache[k];
	}
	
	gOutput[dispatchThreadID.xy] = blurColor;
}

[numthreads(1, N, 1)]
void VertBlurCS(int3 groupThreadID : SV_GroupThreadID,
				int3 dispatchThreadID : SV_DispatchThreadID)
{
	float weights[11] = { w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10 };

	// ʹ�ù����ڴ������ٴ���
	// ģ��N�����أ���Ҫ����N + 2*BlurRadius������

	// ���߳�������N���߳�
	// Ϊ�˻�ö����2*BlurRadius�����أ���Ҫ2*BlurRadius���߳����������������
	if(groupThreadID.y < gBlurRadius) // ���ϱߵ�gBlurRadius���߳�
	{
		// ����ͼ���Ե
		int y = max(dispatchThreadID.y - gBlurRadius, 0);
		gCache[groupThreadID.y] = gInput[int2(dispatchThreadID.x, y)];
	}
	if(groupThreadID.y >= N-gBlurRadius) // ���±ߵ�gBlurRadius���߳�
	{
		// ����ͼ���Ե
		int y = min(dispatchThreadID.y + gBlurRadius, gInput.Length.y-1);
		gCache[groupThreadID.y+2*gBlurRadius] = gInput[int2(dispatchThreadID.x, y)];
	}
	
	gCache[groupThreadID.y+gBlurRadius] = gInput[min(dispatchThreadID.xy, gInput.Length.xy-1)];


	// �ȴ������߳����
	GroupMemoryBarrierWithGroupSync();
	
	//
	// ģ������
	//

	float4 blurColor = float4(0, 0, 0, 0);
	
	for(int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.y + gBlurRadius + i;
		
		blurColor += weights[i+gBlurRadius]*gCache[k];
	}
	
	gOutput[dispatchThreadID.xy] = blurColor;
}