Texture2D gInput : register(t0);
RWTexture2D<float4> gOutput : register(u0);

[numthreads(16, 16, 1)]
void InverseCS(int3 dispatchThreadID : SV_DispatchThreadID)
{
	gOutput[dispatchThreadID.xy] = 1.0f - gInput[dispatchThreadID.xy];
}