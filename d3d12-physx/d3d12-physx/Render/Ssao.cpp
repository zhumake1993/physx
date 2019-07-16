#include "Ssao.h"
#include <DirectXPackedVector.h>

using namespace DirectX::PackedVector;

Ssao::Ssao(UINT width, UINT height)
{
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(std::make_unique<UploadBuffer<SsaoConstants>>(gD3D12Device.Get(), 1, true));
	}

	mWidth = width;
	mHeight = height;

	// ��������ͼ�ķֱ�����Ϊ��ȾĿ���һ��
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = mWidth / 2.0f;
	mViewport.Height = mHeight / 2.0f;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, (int)mWidth / 2, (int)mHeight / 2 };

	BuildResource();
	BuildRandomVectorTexture(); // ע�⣺Ҫ�ȴ�����Դ���ٴ�����������������Դ�ᱻ��գ�����
	BuildRootSignature();
	BuildDescriptor();
	BuildShader();
	BuildPSO();

	BuildOffsetVector();
}

UINT Ssao::SsaoMapWidth()const
{
	return mWidth / 2;
}

UINT Ssao::SsaoMapHeight()const
{
	return mHeight / 2;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE Ssao::Srv()
{
	return mhAmbientMap0GpuSrv;
}

ID3D12DescriptorHeap* Ssao::GetSrvDescriptorHeapPtr()
{
	return mCbvSrvUavDescriptorHeap.Get();
}

void Ssao::DrawNormalsAndDepth()
{
	// ת��״̬��RENDER_TARGET.
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mNormalMap.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));



	//�����ӿںͼ��þ��Ρ�ÿ������ָ���б��Ҫ�����ӿںͼ��þ���
	gCommandList->RSSetViewports(1, &gScreenViewport);
	gCommandList->RSSetScissorRects(1, &gScissorRect);

	// �����Ļ��������ͼ����Ȼ���
	float clearValue[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	gCommandList->ClearRenderTargetView(mhNormalMapCpuRtv, clearValue, 0, nullptr);
	gCommandList->ClearDepthStencilView(mhDepthMapCpuDsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// ָ����ȾĿ��
	gCommandList->OMSetRenderTargets(1, &mhNormalMapCpuRtv, true, &mhDepthMapCpuDsv);



	// ���ø�ǩ��
	gCommandList->SetGraphicsRootSignature(gRootSignatures["main"].Get());

	// �󶨳�������
	auto passCB = gPassCB->GetCurrResource()->Resource();
	gCommandList->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());

	// �����в��ʡ����ڽṹ�����壬���ǿ����ƹ��ѣ�ʹ�ø�������
	auto matBuffer = gMaterialManager->CurrResource();
	gCommandList->SetGraphicsRootShaderResourceView(2, matBuffer->GetGPUVirtualAddress());

	// ����������
	ID3D12DescriptorHeap* descriptorHeaps[] = { gTextureManager->GetSrvDescriptorHeapPtr() };
	gCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// �����е�����
	gCommandList->SetGraphicsRootDescriptorTable(3, gTextureManager->GetGpuSrvTex());

	// ���������������ͼ
	gCommandList->SetGraphicsRootDescriptorTable(4, gTextureManager->GetGpuSrvCube());



	gCommandList->SetPipelineState(gPSOs["DrawNormals"].Get());
	gInstanceManager->Draw((int)RenderLayer::Opaque);
	gInstanceManager->Draw((int)RenderLayer::OpaqueDynamicReflectors);

	// ת��״̬��GENERIC_READ��ʹ���ܹ���shader�ж�ȡ������
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mNormalMap.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void Ssao::UpdateSsaoConstantData(PassConstants& mainPassCB)
{
	SsaoConstants ssaoCB;

	XMMATRIX P = gCamera->GetProj();

	// ��NDC�ռ�[-1,+1]^2ת��������ռ�[0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	ssaoCB.Proj = mainPassCB.Proj;
	ssaoCB.InvProj = mainPassCB.InvProj;
	XMStoreFloat4x4(&ssaoCB.ProjTex, XMMatrixTranspose(P * T));

	std::copy(&mOffsets[0], &mOffsets[14], &ssaoCB.OffsetVectors[0]);

	auto blurWeights = CalcGaussWeights(2.5f);
	ssaoCB.BlurWeights[0] = XMFLOAT4(&blurWeights[0]);
	ssaoCB.BlurWeights[1] = XMFLOAT4(&blurWeights[4]);
	ssaoCB.BlurWeights[2] = XMFLOAT4(&blurWeights[8]);

	ssaoCB.InvRenderTargetSize = XMFLOAT2(1.0f / SsaoMapWidth(), 1.0f / SsaoMapHeight());

	// �ڱβ���
	ssaoCB.OcclusionRadius = 0.5f;
	ssaoCB.OcclusionFadeStart = 0.2f;
	ssaoCB.OcclusionFadeEnd = 1.0f;
	ssaoCB.SurfaceEpsilon = 0.05f;

	mFrameResources[gCurrFrameResourceIndex]->CopyData(0, ssaoCB);
}

void Ssao::ComputeSsao(int blurCount)
{
	// �������ԭʼSSAO����AmbientMap0
	// ת��״̬��RENDER_TARGET.
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mAmbientMap0.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));



	//�����ӿںͼ��þ���
	gCommandList->RSSetViewports(1, &mViewport);
	gCommandList->RSSetScissorRects(1, &mScissorRect);

	// �����ȾĿ��
	float clearValue[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	gCommandList->ClearRenderTargetView(mhAmbientMap0CpuRtv, clearValue, 0, nullptr);

	// ָ����ȾĿ��
	gCommandList->OMSetRenderTargets(1, &mhAmbientMap0CpuRtv, true, nullptr);



	// ���ø�ǩ��
	gCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	// �󶨳�������
	auto uploadBuffer = mFrameResources[gCurrFrameResourceIndex]->Resource();
	gCommandList->SetGraphicsRootConstantBufferView(0, uploadBuffer->GetGPUVirtualAddress());
	gCommandList->SetGraphicsRoot32BitConstant(1, 0, 0);

	// ����������
	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvSrvUavDescriptorHeap.Get() };
	gCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// �󶨷�������ͼ
	gCommandList->SetGraphicsRootDescriptorTable(2, mhNormalMapGpuSrv);

	// �������ͼ
	gCommandList->SetGraphicsRootDescriptorTable(3, mhDepthMapGpuSrv);

	// �����������ͼ
	gCommandList->SetGraphicsRootDescriptorTable(4, mhRandomVectorMapGpuSrv);

	gCommandList->SetPipelineState(gPSOs["Ssao"].Get());

	// ����ȫ��quad
	gCommandList->IASetVertexBuffers(0, 0, nullptr);
	gCommandList->IASetIndexBuffer(nullptr);
	gCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gCommandList->DrawInstanced(6, 1, 0, 0);

	// ״̬ת��ΪGENERIC_READ��ʹ�����ܹ���shader��ȡ
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mAmbientMap0.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));

	BlurAmbientMap(blurCount);
}

void Ssao::BlurAmbientMap(int blurCount)
{
	gCommandList->SetPipelineState(gPSOs["SsaoBlur"].Get());

	auto uploadBuffer = mFrameResources[gCurrFrameResourceIndex]->Resource();
	gCommandList->SetGraphicsRootConstantBufferView(0, uploadBuffer->GetGPUVirtualAddress());

	for (int i = 0; i < blurCount; ++i) {
		BlurAmbientMap(true);
		BlurAmbientMap(false);
	}
}

void Ssao::BlurAmbientMap(bool horzBlur)
{
	ID3D12Resource* output = nullptr;
	CD3DX12_GPU_DESCRIPTOR_HANDLE inputSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE outputRtv;

	if (horzBlur == true) {
		output = mAmbientMap1.Get();
		inputSrv = mhAmbientMap0GpuSrv;
		outputRtv = mhAmbientMap1CpuRtv;
		gCommandList->SetGraphicsRoot32BitConstant(1, 1, 0);
	} else {
		output = mAmbientMap0.Get();
		inputSrv = mhAmbientMap1GpuSrv;
		outputRtv = mhAmbientMap0CpuRtv;
		gCommandList->SetGraphicsRoot32BitConstant(1, 0, 0);
	}

	// ת��״̬��RENDER_TARGET.
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(output,
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// �����ȾĿ��
	float clearValue[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	gCommandList->ClearRenderTargetView(outputRtv, clearValue, 0, nullptr);

	// ָ����ȾĿ��
	gCommandList->OMSetRenderTargets(1, &outputRtv, true, nullptr);

	// ��ǩ������

	// ������/�����ͼ�Դ��ڰ�״̬

	// �����뻷����ͼ
	gCommandList->SetGraphicsRootDescriptorTable(4, inputSrv);

	// ����ȫ��quad
	gCommandList->IASetVertexBuffers(0, 0, nullptr);
	gCommandList->IASetIndexBuffer(nullptr);
	gCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gCommandList->DrawInstanced(6, 1, 0, 0);

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(output,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void Ssao::BuildResource()
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = mNormalMapFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	float normalClearColor[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	CD3DX12_CLEAR_VALUE optClear(mNormalMapFormat, normalClearColor);
	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&mNormalMap)));

	// ������ͼ�ķֱ��ʼ���
	texDesc.Width = mWidth / 2;
	texDesc.Height = mHeight / 2;
	texDesc.Format = mAmbientMapFormat;

	float ambientClearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	optClear = CD3DX12_CLEAR_VALUE(mAmbientMapFormat, ambientClearColor);

	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&mAmbientMap0)));

	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&mAmbientMap1)));

	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = mWidth;
	depthStencilDesc.Height = mHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = gDepthStencilFormat;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear2;
	optClear2.Format = gDepthStencilFormat;
	optClear2.DepthStencil.Depth = 1.0f;
	optClear2.DepthStencil.Stencil = 0;
	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optClear2,
		IID_PPV_ARGS(mDepthMap.GetAddressOf())));
}

void Ssao::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE texTable0;
	texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable2;
	texTable2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[5];

	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstants(1, 1);
	slotRootParameter[2].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL); // ��������ͼ
	slotRootParameter[3].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL); // �����ͼ
	slotRootParameter[4].InitAsDescriptorTable(1, &texTable2, D3D12_SHADER_VISIBILITY_PIXEL); // ���������ͼ

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC depthMapSam(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,
		0,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE);

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	std::array<CD3DX12_STATIC_SAMPLER_DESC, 4> staticSamplers =
	{
		pointClamp, linearClamp, depthMapSam, linearWrap
	};

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

void Ssao::BuildDescriptor()
{
	// ����SRV��
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 5;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(gD3D12Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mCbvSrvUavDescriptorHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor(mCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor(mCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	// ����������������
	mhNormalMapCpuSrv = hCpuDescriptor;
	mhDepthMapCpuSrv = hCpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	mhRandomVectorMapCpuSrv = hCpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	mhAmbientMap0CpuSrv = hCpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	mhAmbientMap1CpuSrv = hCpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);

	mhNormalMapGpuSrv = hGpuDescriptor;
	mhDepthMapGpuSrv = hGpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	mhRandomVectorMapGpuSrv = hGpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	mhAmbientMap0GpuSrv = hGpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	mhAmbientMap1GpuSrv = hGpuDescriptor.Offset(1, gCbvSrvUavDescriptorSize);

	// ����������
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	srvDesc.Format = mNormalMapFormat;
	gD3D12Device->CreateShaderResourceView(mNormalMap.Get(), &srvDesc, mhNormalMapCpuSrv);

	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	gD3D12Device->CreateShaderResourceView(mDepthMap.Get(), &srvDesc, mhDepthMapCpuSrv);

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	gD3D12Device->CreateShaderResourceView(mRandomVectorMap.Get(), &srvDesc, mhRandomVectorMapCpuSrv);

	srvDesc.Format = mAmbientMapFormat;
	gD3D12Device->CreateShaderResourceView(mAmbientMap0.Get(), &srvDesc, mhAmbientMap0CpuSrv);
	gD3D12Device->CreateShaderResourceView(mAmbientMap1.Get(), &srvDesc, mhAmbientMap1CpuSrv);

	// ����RTV��
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = 3;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(gD3D12Device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtvDescriptor(mRtvHeap->GetCPUDescriptorHandleForHeapStart());

	// ����������������
	mhNormalMapCpuRtv = hCpuRtvDescriptor;
	mhAmbientMap0CpuRtv = hCpuRtvDescriptor.Offset(1, gRtvDescriptorSize);
	mhAmbientMap1CpuRtv = hCpuRtvDescriptor.Offset(1, gRtvDescriptorSize);

	// ����������
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.Texture2D.PlaneSlice = 0;

	rtvDesc.Format = mNormalMapFormat;
	gD3D12Device->CreateRenderTargetView(mNormalMap.Get(), &rtvDesc, mhNormalMapCpuRtv);

	rtvDesc.Format = mAmbientMapFormat;
	gD3D12Device->CreateRenderTargetView(mAmbientMap0.Get(), &rtvDesc, mhAmbientMap0CpuRtv);
	gD3D12Device->CreateRenderTargetView(mAmbientMap1.Get(), &rtvDesc, mhAmbientMap1CpuRtv);

	// ����DSV��
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(gD3D12Device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));

	mhDepthMapCpuDsv = CD3DX12_CPU_DESCRIPTOR_HANDLE(mDsvHeap->GetCPUDescriptorHandleForHeapStart(), 0, gDsvDescriptorSize);

	// �������ģ����ͼ
	gD3D12Device->CreateDepthStencilView(mDepthMap.Get(), nullptr, mhDepthMapCpuDsv);
}

void Ssao::BuildShader()
{
	gShaders["drawNormalsVS"] = d3dUtil::CompileShader(L"Shaders\\DrawNormals.hlsl", nullptr, "VS", "vs_5_1");
	gShaders["drawNormalsPS"] = d3dUtil::CompileShader(L"Shaders\\DrawNormals.hlsl", nullptr, "PS", "ps_5_1");

	gShaders["ssaoVS"] = d3dUtil::CompileShader(L"Shaders\\Ssao.hlsl", nullptr, "VS", "vs_5_1");
	gShaders["ssaoPS"] = d3dUtil::CompileShader(L"Shaders\\Ssao.hlsl", nullptr, "PS", "ps_5_1");

	gShaders["ssaoBlurVS"] = d3dUtil::CompileShader(L"Shaders\\SsaoBlur.hlsl", nullptr, "VS", "vs_5_1");
	gShaders["ssaoBlurPS"] = d3dUtil::CompileShader(L"Shaders\\SsaoBlur.hlsl", nullptr, "PS", "ps_5_1");
}

void Ssao::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC basePsoDesc;
	ZeroMemory(&basePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	basePsoDesc.InputLayout = { gInputLayout.data(), (UINT)gInputLayout.size() };
	basePsoDesc.pRootSignature = gRootSignatures["main"].Get();
	basePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(gShaders["standardVS"]->GetBufferPointer()),
		gShaders["standardVS"]->GetBufferSize()
	};
	basePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(gShaders["opaquePS"]->GetBufferPointer()),
		gShaders["opaquePS"]->GetBufferSize()
	};
	basePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	basePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	basePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	basePsoDesc.SampleMask = UINT_MAX;
	basePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	basePsoDesc.NumRenderTargets = 1;
	basePsoDesc.RTVFormats[0] = gBackBufferFormat;
	basePsoDesc.SampleDesc.Count = g4xMsaaState ? 4 : 1;
	basePsoDesc.SampleDesc.Quality = g4xMsaaState ? (g4xMsaaQuality - 1) : 0;
	basePsoDesc.DSVFormat = gDepthStencilFormat;

	// ���Ʒ�����
	D3D12_GRAPHICS_PIPELINE_STATE_DESC drawNormalsPsoDesc = basePsoDesc;
	drawNormalsPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(gShaders["drawNormalsVS"]->GetBufferPointer()),
		gShaders["drawNormalsVS"]->GetBufferSize()
	};
	drawNormalsPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(gShaders["drawNormalsPS"]->GetBufferPointer()),
		gShaders["drawNormalsPS"]->GetBufferSize()
	};
	drawNormalsPsoDesc.RTVFormats[0] = mNormalMapFormat;
	drawNormalsPsoDesc.SampleDesc.Count = 1;
	drawNormalsPsoDesc.SampleDesc.Quality = 0;
	drawNormalsPsoDesc.DSVFormat = gDepthStencilFormat;
	ThrowIfFailed(gD3D12Device->CreateGraphicsPipelineState(&drawNormalsPsoDesc, IID_PPV_ARGS(&gPSOs["DrawNormals"])));

	// SSAO.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC ssaoPsoDesc = basePsoDesc;
	ssaoPsoDesc.InputLayout = { nullptr, 0 };
	ssaoPsoDesc.pRootSignature = mRootSignature.Get();
	ssaoPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(gShaders["ssaoVS"]->GetBufferPointer()),
		gShaders["ssaoVS"]->GetBufferSize()
	};
	ssaoPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(gShaders["ssaoPS"]->GetBufferPointer()),
		gShaders["ssaoPS"]->GetBufferSize()
	};

	// SSAO����Ҫ��Ȼ���
	ssaoPsoDesc.DepthStencilState.DepthEnable = false;
	ssaoPsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	ssaoPsoDesc.RTVFormats[0] = mAmbientMapFormat;
	ssaoPsoDesc.SampleDesc.Count = 1;
	ssaoPsoDesc.SampleDesc.Quality = 0;
	ssaoPsoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
	ThrowIfFailed(gD3D12Device->CreateGraphicsPipelineState(&ssaoPsoDesc, IID_PPV_ARGS(&gPSOs["Ssao"])));

	// SSAOģ��
	D3D12_GRAPHICS_PIPELINE_STATE_DESC ssaoBlurPsoDesc = ssaoPsoDesc;
	ssaoBlurPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(gShaders["ssaoBlurVS"]->GetBufferPointer()),
		gShaders["ssaoBlurVS"]->GetBufferSize()
	};
	ssaoBlurPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(gShaders["ssaoBlurPS"]->GetBufferPointer()),
		gShaders["ssaoBlurPS"]->GetBufferSize()
	};
	ThrowIfFailed(gD3D12Device->CreateGraphicsPipelineState(&ssaoBlurPsoDesc, IID_PPV_ARGS(&gPSOs["SsaoBlur"])));
}

void Ssao::BuildOffsetVector()
{
	// 14�����ȷֲ���������8��������Ķ����6������������е�
	// ÿ��ö�������෴�Ķ��㣬����һ������ʹʹ������14�����㣬������������Ȼ��ɢ

	// 8��������Ķ���
	mOffsets[0] = XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f);
	mOffsets[1] = XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);

	mOffsets[2] = XMFLOAT4(-1.0f, +1.0f, +1.0f, 0.0f);
	mOffsets[3] = XMFLOAT4(+1.0f, -1.0f, -1.0f, 0.0f);

	mOffsets[4] = XMFLOAT4(+1.0f, +1.0f, -1.0f, 0.0f);
	mOffsets[5] = XMFLOAT4(-1.0f, -1.0f, +1.0f, 0.0f);

	mOffsets[6] = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
	mOffsets[7] = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);

	// 6������������е�
	mOffsets[8] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	mOffsets[9] = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f);

	mOffsets[10] = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	mOffsets[11] = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f);

	mOffsets[12] = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	mOffsets[13] = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f);

	for (int i = 0; i < 14; ++i) {
		// ������[0.25, 1.0]�����
		float s = MathHelper::RandF(0.25f, 1.0f);

		XMVECTOR v = s * XMVector4Normalize(XMLoadFloat4(&mOffsets[i]));

		XMStoreFloat4(&mOffsets[i], v);
	}
}

void Ssao::BuildRandomVectorTexture()
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = 256;
	texDesc.Height = 256;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mRandomVectorMap)));

	// Ϊ�˽�CPU�ڴ����ݿ�����Ĭ�ϻ��壬��Ҫһ���м����ػ���

	const UINT num2DSubresources = texDesc.DepthOrArraySize * texDesc.MipLevels;
	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(mRandomVectorMap.Get(), 0, num2DSubresources);

	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(mRandomVectorMapUploadBuffer.GetAddressOf())));

	XMCOLOR initData[256 * 256];
	for (int i = 0; i < 256; ++i) {
		for (int j = 0; j < 256; ++j) {
			// ���������ȡֵ��[0,1]�У�shader�л�ӳ����[-1,1]
			XMFLOAT3 v(MathHelper::RandF(), MathHelper::RandF(), MathHelper::RandF());

			initData[i * 256 + j] = XMCOLOR(v.x, v.y, v.z, 0.0f);
		}
	}

	D3D12_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pData = initData;
	subResourceData.RowPitch = 256 * sizeof(XMCOLOR);
	subResourceData.SlicePitch = subResourceData.RowPitch * 256;

	// ע�⣬mRandomVectorMap��״̬ת��Ϊ��GENERIC_READ��ʹ�����ܹ���shader��ȡ
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRandomVectorMap.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));
	UpdateSubresources(gCommandList.Get(), mRandomVectorMap.Get(), mRandomVectorMapUploadBuffer.Get(),
		0, 0, num2DSubresources, &subResourceData);
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRandomVectorMap.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

std::vector<float> Ssao::CalcGaussWeights(float sigma)
{
	float twoSigma2 = 2.0f * sigma * sigma;

	// ʹ��sigma������ģ���뾶����Ϊsigma�������ߵĿ��
	int blurRadius = (int)ceil(2.0f * sigma);

	assert(blurRadius <= mMaxBlurRadius);

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