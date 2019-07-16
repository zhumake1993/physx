#include "BlurFilter.h"

BlurFilter::BlurFilter(UINT width, UINT height, DXGI_FORMAT format)
{
	mWidth = width;
	mHeight = height;
	mFormat = format;

	BuildResource();
	BuildRootSignature();
	BuildDescriptor();
	BuildShader();
	BuildPSO();
}

void BlurFilter::CopyIn(ID3D12Resource* input)
{
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBlurTex0.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

	gCommandList->CopyResource(mBlurTex0.Get(), input);

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBlurTex0.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void BlurFilter::Execute(int blurCount)
{
	auto weights = CalcGaussWeights(2.5f);
	int blurRadius = (int)weights.size() / 2;

	// ע�⣺����ʹ������һ���������ѣ���Ҫ���µ���SetDescriptorHeaps
	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvSrvUavDescriptorHeap.Get() };
	gCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	gCommandList->SetComputeRootSignature(mRootSignature.Get());

	gCommandList->SetComputeRoot32BitConstants(0, 1, &blurRadius, 0);
	gCommandList->SetComputeRoot32BitConstants(0, (UINT)weights.size(), weights.data(), 1);

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBlurTex1.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	for (int i = 0; i < blurCount; ++i) {
		//
		// ˮƽģ��
		//

		gCommandList->SetPipelineState(gPSOs["HorzBlur"].Get());

		gCommandList->SetComputeRootDescriptorTable(1, mBlur0GpuSrv);
		gCommandList->SetComputeRootDescriptorTable(2, mBlur1GpuUav);

		// ��Ҫ�ַ�������������һ������
		// ÿ�鸲��256�����أ�256�����ڼ�����ɫ���У�
		UINT numGroupsX = (UINT)ceilf(mWidth / 256.0f);
		gCommandList->Dispatch(numGroupsX, mHeight, 1);

		gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBlurTex0.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBlurTex1.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));

		//
		// ��ֱģ��
		//

		gCommandList->SetPipelineState(gPSOs["VertBlur"].Get());

		gCommandList->SetComputeRootDescriptorTable(1, mBlur1GpuSrv);
		gCommandList->SetComputeRootDescriptorTable(2, mBlur0GpuUav);

		// ��Ҫ�ַ�������������һ������
		// ÿ�鸲��256�����أ�256�����ڼ�����ɫ���У�
		UINT numGroupsY = (UINT)ceilf(mHeight / 256.0f);
		gCommandList->Dispatch(mWidth, numGroupsY, 1);

		gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBlurTex0.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));

		gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBlurTex1.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
	}

	// Ҫ��״̬�ָ���D3D12_RESOURCE_STATE_GENERIC_READ
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBlurTex1.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void BlurFilter::CopyOut(ID3D12Resource* output)
{
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(output,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST));

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBlurTex0.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE));

	gCommandList->CopyResource(output, mBlurTex0.Get());

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(output,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET));

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mBlurTex0.Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void BlurFilter::ExcuteInOut(ID3D12Resource* input, ID3D12Resource* output, int blurCount)
{
	CopyIn(input);
	Execute(blurCount);
	CopyOut(output);
}

void BlurFilter::BuildResource()
{
	// ע�⣺ѹ����ʽ������UAV
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = mFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mBlurTex0)));

	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mBlurTex1)));
}

void BlurFilter::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE srvTable;
	srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE uavTable;
	uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[3];

	slotRootParameter[0].InitAsConstants(12, 0);
	slotRootParameter[1].InitAsDescriptorTable(1, &srvTable);
	slotRootParameter[2].InitAsDescriptorTable(1, &uavTable);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr) {
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(gD3D12Device->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

void BlurFilter::BuildDescriptor()
{
	// ����SRV��
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 4;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(gD3D12Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mCbvSrvUavDescriptorHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor(mCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor(mCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	// ����������������
	mBlur0CpuSrv = hCpuDescriptor;
	mBlur0CpuUav = hCpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	mBlur1CpuSrv = hCpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	mBlur1CpuUav = hCpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);

	mBlur0GpuSrv = hGpuDescriptor;
	mBlur0GpuUav = hGpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	mBlur1GpuSrv = hGpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	mBlur1GpuUav = hGpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);

	// ����������
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = mFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = mFormat;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	gD3D12Device->CreateShaderResourceView(mBlurTex0.Get(), &srvDesc, mBlur0CpuSrv);
	gD3D12Device->CreateUnorderedAccessView(mBlurTex0.Get(), nullptr, &uavDesc, mBlur0CpuUav);

	gD3D12Device->CreateShaderResourceView(mBlurTex1.Get(), &srvDesc, mBlur1CpuSrv);
	gD3D12Device->CreateUnorderedAccessView(mBlurTex1.Get(), nullptr, &uavDesc, mBlur1CpuUav);
}

void BlurFilter::BuildShader()
{
	gShaders["HorzBlurCS"] = d3dUtil::CompileShader(L"Shaders\\Blur.hlsl", nullptr, "HorzBlurCS", "cs_5_1");
	gShaders["VertBlurCS"] = d3dUtil::CompileShader(L"Shaders\\Blur.hlsl", nullptr, "VertBlurCS", "cs_5_1");
}

void BlurFilter::BuildPSO()
{
	// ˮƽģ��
	D3D12_COMPUTE_PIPELINE_STATE_DESC horzBlurPSO = {};
	horzBlurPSO.pRootSignature = mRootSignature.Get();
	horzBlurPSO.CS =
	{
		reinterpret_cast<BYTE*>(gShaders["HorzBlurCS"]->GetBufferPointer()),
		gShaders["HorzBlurCS"]->GetBufferSize()
	};
	horzBlurPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	ThrowIfFailed(gD3D12Device->CreateComputePipelineState(&horzBlurPSO, IID_PPV_ARGS(&gPSOs["HorzBlur"])));

	// ��ֱģ��
	D3D12_COMPUTE_PIPELINE_STATE_DESC vertBlurPSO = {};
	vertBlurPSO.pRootSignature = mRootSignature.Get();
	vertBlurPSO.CS =
	{
		reinterpret_cast<BYTE*>(gShaders["VertBlurCS"]->GetBufferPointer()),
		gShaders["VertBlurCS"]->GetBufferSize()
	};
	vertBlurPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	ThrowIfFailed(gD3D12Device->CreateComputePipelineState(&vertBlurPSO, IID_PPV_ARGS(&gPSOs["VertBlur"])));
}

std::vector<float> BlurFilter::CalcGaussWeights(float sigma)
{
	float twoSigma2 = 2.0f * sigma * sigma;

	// ʹ��sigma������ģ���뾶����Ϊsigma�������ߵĿ��
	int blurRadius = (int)ceil(2.0f * sigma);

	assert(blurRadius <= MaxBlurRadius);

	std::vector<float> weights;
	weights.resize(2 * blurRadius + 1);

	float weightSum = 0.0f;

	for (int i = -blurRadius; i <= blurRadius; ++i) {
		float x = (float)i;

		weights[i + blurRadius] = expf(-x * x / twoSigma2);

		weightSum += weights[i + blurRadius];
	}

	for (int i = 0; i < weights.size(); ++i) {
		weights[i] /= weightSum;
	}

	return weights;
}