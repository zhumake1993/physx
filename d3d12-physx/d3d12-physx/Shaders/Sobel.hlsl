//=============================================================================
// ʹ��Sobel���ӽ��б�Ե���
//=============================================================================

Texture2D gInput            : register(t0);
RWTexture2D<float4> gOutput : register(u0);


// ��RGBֵ���Ƽ������ȣ���ЩȨ�ػ���ʵ�飬ʵ������۾��Թ�Ĳ�ͬ���������ж�
float CalcLuminance(float3 color)
{
    return dot(color, float3(0.299f, 0.587f, 0.114f));
}

[numthreads(16, 16, 1)]
void SobelCS(int3 dispatchThreadID : SV_DispatchThreadID)
{
    // ���������ص���Χ����
	float4 c[3][3];
	for(int i = 0; i < 3; ++i)
	{
		for(int j = 0; j < 3; ++j)
		{
			int2 xy = dispatchThreadID.xy + int2(-1 + j, -1 + i);
			c[i][j] = gInput[xy]; 
		}
	}

	// ����ÿ����ɫͨ����ʹ��Sobel����xƫ��
	float4 Gx = -1.0f*c[0][0] - 2.0f*c[1][0] - 1.0f*c[2][0] + 1.0f*c[0][2] + 2.0f*c[1][2] + 1.0f*c[2][2];

	// ����ÿ����ɫͨ����ʹ��Sobel����yƫ��
	float4 Gy = -1.0f*c[2][0] - 2.0f*c[2][1] - 1.0f*c[2][1] + 1.0f*c[0][0] + 2.0f*c[0][1] + 1.0f*c[0][2];

	// �ݶ���(Gx, Gy)������ÿ����ɫͨ��������magnitude���õ����仯��
	float4 mag = sqrt(Gx*Gx + Gy*Gy);

	// ��Ե��ɫ���Ǳ�Ե��ɫ
	mag = saturate(CalcLuminance(mag.rgb));

	gOutput[dispatchThreadID.xy] = mag;
}
