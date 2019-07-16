Texture2D gInput0 : register(t0);
Texture2D gInput1 : register(t1);
RWTexture2D<float4> gOutput : register(u0);

[numthreads(16, 16, 1)]
void MultiplyCS(int3 dispatchThreadID : SV_DispatchThreadID)
{
	gOutput[dispatchThreadID.xy] = gInput0[dispatchThreadID.xy] * gInput1[dispatchThreadID.xy];
}