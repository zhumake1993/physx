#include "MultiplyFilter.h"

MultiplyFilter::MultiplyFilter(UINT width, UINT height, DXGI_FORMAT format)
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

void MultiplyFilter::CopyIn0(ID3D12Resource* input)
{
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mInput0.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

	gCommandList->CopyResource(mInput0.Get(), input);

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mInput0.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void MultiplyFilter::CopyIn1(ID3D12Resource* input)
{
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mInput1.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

	gCommandList->CopyResource(mInput1.Get(), input);

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mInput1.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void MultiplyFilter::Execute()
{
	// 注意：由于使用了另一个描述符堆，需要重新调用SetDescriptorHeaps
	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvSrvUavDescriptorHeap.Get() };
	gCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	gCommandList->SetComputeRootSignature(mRootSignature.Get());

	gCommandList->SetPipelineState(gPSOs["Multiply"].Get());

	gCommandList->SetComputeRootDescriptorTable(0, mInput0GpuSrv);
	gCommandList->SetComputeRootDescriptorTable(1, mInput1GpuSrv);
	gCommandList->SetComputeRootDescriptorTable(2, mOutputGpuUav);

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mOutput.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

	UINT numGroupsX = (UINT)ceilf(mWidth / 16.0f);
	UINT numGroupsY = (UINT)ceilf(mHeight / 16.0f);
	gCommandList->Dispatch(numGroupsX, numGroupsY, 1);

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mOutput.Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void MultiplyFilter::CopyOut(ID3D12Resource* output)
{
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(output,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST));

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mOutput.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE));

	gCommandList->CopyResource(output, mOutput.Get());

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(output,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET));

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mOutput.Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void MultiplyFilter::ExcuteInOut(ID3D12Resource* input0, ID3D12Resource* input1, ID3D12Resource* output)
{
	CopyIn0(input0);
	CopyIn1(input1);
	Execute();
	CopyOut(output);
}

void MultiplyFilter::BuildResource()
{
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
		IID_PPV_ARGS(&mOutput)));

	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mInput0)));

	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mInput1)));
}

void MultiplyFilter::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE srvTable0;
	srvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE srvTable1;
	srvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

	CD3DX12_DESCRIPTOR_RANGE uavTable;
	uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[3];

	slotRootParameter[0].InitAsDescriptorTable(1, &srvTable0);
	slotRootParameter[1].InitAsDescriptorTable(1, &srvTable1);
	slotRootParameter[2].InitAsDescriptorTable(1, &uavTable);

	auto staticSamplers = d3dUtil::GetStaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(_countof(slotRootParameter), slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
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

void MultiplyFilter::BuildDescriptor()
{
	// 创建SRV堆
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 3;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(gD3D12Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mCbvSrvUavDescriptorHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor(mCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor(mCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	// 保留描述符的引用
	mInput0CpuSrv = hCpuDescriptor;
	mInput1CpuSrv = hCpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	mOutputCpuUav = hCpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);

	mInput0GpuSrv = hGpuDescriptor;
	mInput1GpuSrv = hGpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	mOutputGpuUav = hGpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);

	// 创建描述符
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

	gD3D12Device->CreateShaderResourceView(mInput0.Get(), &srvDesc, mInput0CpuSrv);
	gD3D12Device->CreateShaderResourceView(mInput1.Get(), &srvDesc, mInput1CpuSrv);
	gD3D12Device->CreateUnorderedAccessView(mOutput.Get(), nullptr, &uavDesc, mOutputCpuUav);
}

void MultiplyFilter::BuildShader()
{
	gShaders["MultiplyCS"] = d3dUtil::CompileShader(L"Shaders\\Multiply.hlsl", nullptr, "MultiplyCS", "cs_5_1");
}

void MultiplyFilter::BuildPSO()
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC multiplyCSPSODesc = {};
	multiplyCSPSODesc.pRootSignature = mRootSignature.Get();
	multiplyCSPSODesc.CS =
	{
		reinterpret_cast<BYTE*>(gShaders["MultiplyCS"]->GetBufferPointer()),
		gShaders["MultiplyCS"]->GetBufferSize()
	};
	multiplyCSPSODesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	ThrowIfFailed(gD3D12Device->CreateComputePipelineState(&multiplyCSPSODesc, IID_PPV_ARGS(&gPSOs["Multiply"])));
}