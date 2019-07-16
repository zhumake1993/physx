#include "CubeMap.h"

CubeMap::CubeMap(DXGI_FORMAT format, DXGI_FORMAT depthStencilFormat)
{
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(std::make_unique<UploadBuffer<PassConstants>>(gD3D12Device.Get(), 6, true));
	}

	mWidth = mCubeMapSize;
	mHeight = mCubeMapSize;
	mFormat = format;
	mDepthStencilFormat = depthStencilFormat;

	mViewport = { 0.0f, 0.0f, (float)mWidth, (float)mHeight, 0.0f, 1.0f };
	mScissorRect = { 0, 0, (int)mWidth, (int)mHeight };

	BuildResource();
	BuildDescriptor();
}

void CubeMap::SetShadow(ID3D12DescriptorHeap* srvDescriptorHeapPtr, CD3DX12_GPU_DESCRIPTOR_HANDLE srv)
{
	mShadowSrvDescriptorHeapPtr = srvDescriptorHeapPtr;
	mShadowSrv = srv;
}

void CubeMap::SetSsao(ID3D12DescriptorHeap* srvDescriptorHeapPtr, CD3DX12_GPU_DESCRIPTOR_HANDLE srv)
{
	mSsaoSrvDescriptorHeapPtr = srvDescriptorHeapPtr;
	mSsaoSrv = srv;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE CubeMap::Srv()
{
	return mhGpuSrv;
}

ID3D12DescriptorHeap* CubeMap::GetSrvDescriptorHeapPtr()
{
	return mCbvSrvUavDescriptorHeap.Get();;
}

void CubeMap::BuildCubeFaceCamera(float x, float y, float z)
{
	// 立方体贴图的中心点
	XMFLOAT3 center(x, y, z);

	// 朝向
	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(x + 1.0f, y, z), // +X
		XMFLOAT3(x - 1.0f, y, z), // -X
		XMFLOAT3(x, y + 1.0f, z), // +Y
		XMFLOAT3(x, y - 1.0f, z), // -Y
		XMFLOAT3(x, y, z + 1.0f), // +Z
		XMFLOAT3(x, y, z - 1.0f)  // -Z
	};

	// up向量
	XMFLOAT3 ups[6] =
	{
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
		XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
		XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
		XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
		XMFLOAT3(0.0f, 1.0f, 0.0f)	 // -Z
	};

	for (int i = 0; i < 6; ++i) {
		mCubeMapCamera[i].LookAt(center, targets[i], ups[i]);
		mCubeMapCamera[i].SetLens(0.5f * XM_PI, 1.0f, 0.1f, 1000.0f);
		mCubeMapCamera[i].UpdateViewMatrix();
	}
}

void CubeMap::UpdatePassConstantsData(PassConstants& mainPassCB)
{
	auto& uploadBuffer = mFrameResources[gCurrFrameResourceIndex];

	for (int i = 0; i < 6; ++i) {
		PassConstants cubeFacePassCB = mainPassCB;

		XMMATRIX view = mCubeMapCamera[i].GetView();
		XMMATRIX proj = mCubeMapCamera[i].GetProj();

		XMMATRIX viewProj = XMMatrixMultiply(view, proj);
		XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
		XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
		XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

		XMStoreFloat4x4(&cubeFacePassCB.View, XMMatrixTranspose(view));
		XMStoreFloat4x4(&cubeFacePassCB.InvView, XMMatrixTranspose(invView));
		XMStoreFloat4x4(&cubeFacePassCB.Proj, XMMatrixTranspose(proj));
		XMStoreFloat4x4(&cubeFacePassCB.InvProj, XMMatrixTranspose(invProj));
		XMStoreFloat4x4(&cubeFacePassCB.ViewProj, XMMatrixTranspose(viewProj));
		XMStoreFloat4x4(&cubeFacePassCB.InvViewProj, XMMatrixTranspose(invViewProj));
		cubeFacePassCB.EyePosW = mCubeMapCamera[i].GetPosition3f();
		cubeFacePassCB.RenderTargetSize = XMFLOAT2((float)mCubeMapSize, (float)mCubeMapSize);
		cubeFacePassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mCubeMapSize, 1.0f / mCubeMapSize);

		uploadBuffer->CopyData(i, cubeFacePassCB);
	}
}

void CubeMap::DrawSceneToCubeMap()
{
	gCommandList->RSSetViewports(1, &mViewport);
	gCommandList->RSSetScissorRects(1, &mScissorRect);

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mCubeMap.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));

	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

	// 对于每一个立方体贴图面
	for (int i = 0; i < 6; ++i) {
		// 清空后缓冲和深度缓冲
		gCommandList->ClearRenderTargetView(mhCpuRtv[i], Colors::LightSteelBlue, 0, nullptr);
		gCommandList->ClearDepthStencilView(mhCubeDSV, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		// 指定渲染目标
		gCommandList->OMSetRenderTargets(1, &mhCpuRtv[i], true, &mhCubeDSV);

		// 设置根签名
		gCommandList->SetGraphicsRootSignature(gRootSignatures["main"].Get());

		// 绑定常量缓冲
		auto uploadBuffer = mFrameResources[gCurrFrameResourceIndex]->Resource();
		gCommandList->SetGraphicsRootConstantBufferView(1, uploadBuffer->GetGPUVirtualAddress() + i * passCBByteSize);

		// 绑定所有材质。对于结构化缓冲，我们可以绕过堆，使用根描述符
		auto matBuffer = gMaterialManager->CurrResource();
		gCommandList->SetGraphicsRootShaderResourceView(2, matBuffer->GetGPUVirtualAddress());

		// 绑定描述符堆
		ID3D12DescriptorHeap* descriptorHeaps[] = { gTextureManager->GetSrvDescriptorHeapPtr() };
		gCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		// 绑定所有的纹理
		gCommandList->SetGraphicsRootDescriptorTable(3, gTextureManager->GetGpuSrvTex());

		// 绑定天空球立方体贴图
		gCommandList->SetGraphicsRootDescriptorTable(4, gTextureManager->GetGpuSrvCube());

		// 绑定阴影贴图
		ID3D12DescriptorHeap* descriptorHeapsShadow[] = { mShadowSrvDescriptorHeapPtr };
		gCommandList->SetDescriptorHeaps(_countof(descriptorHeapsShadow), descriptorHeapsShadow);
		gCommandList->SetGraphicsRootDescriptorTable(5, mShadowSrv);

		// 绑定Ssao
		ID3D12DescriptorHeap* descriptorHeapsSsao[] = { mSsaoSrvDescriptorHeapPtr };
		gCommandList->SetDescriptorHeaps(_countof(descriptorHeapsSsao), descriptorHeapsSsao);
		gCommandList->SetGraphicsRootDescriptorTable(6, mSsaoSrv);

		gCommandList->SetPipelineState(gPSOs["opaque"].Get());
		gInstanceManager->Draw((int)RenderLayer::Opaque);

		gCommandList->SetPipelineState(gPSOs["alphaTested"].Get());
		gInstanceManager->Draw((int)RenderLayer::AlphaTested);

		gCommandList->SetPipelineState(gPSOs["transparent"].Get());
		gInstanceManager->Draw((int)RenderLayer::Transparent);

		gCommandList->SetPipelineState(gPSOs["sky"].Get());
		gInstanceManager->Draw((int)RenderLayer::Sky);
	}

	// 将资源状态改回GENERIC_READ，使得能够在着色器中读取纹理
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mCubeMap.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void CubeMap::BuildResource()
{
	D3D12_RESOURCE_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.DepthOrArraySize = 6; // 6个面
	texDesc.MipLevels = 1;
	texDesc.Format = mFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;  // 允许渲染目标

	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&CD3DX12_CLEAR_VALUE(mFormat, DirectX::Colors::LightSteelBlue),
		IID_PPV_ARGS(&mCubeMap)));

	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = mCubeMapSize;
	depthStencilDesc.Height = mCubeMapSize;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = mDepthStencilFormat;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(gD3D12Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optClear,
		IID_PPV_ARGS(mCubeDepthStencilBuffer.GetAddressOf())));
}

void CubeMap::BuildDescriptor()
{
	// 创建SRV堆
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1; // 暂时只有一个动态立方体贴图
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(gD3D12Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mCbvSrvUavDescriptorHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor(mCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor(mCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	// 保留描述符的引用
	mhCpuSrv = hCpuDescriptor;
	mhGpuSrv = hGpuDescriptor;

	// 创建描述符
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = mFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

	// 创建整个立方体贴图资源的SRV
	gD3D12Device->CreateShaderResourceView(mCubeMap.Get(), &srvDesc, mhCpuSrv);



	// 创建RTV堆
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = 6;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(gD3D12Device->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	for (int i = 0; i < 6; ++i)
		mhCpuRtv[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), i, gRtvDescriptorSize);

	// 创建每个立方体面的RTV
	for (int i = 0; i < 6; ++i) {
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY; //纹理2D数组
		rtvDesc.Format = mFormat;
		rtvDesc.Texture2DArray.MipSlice = 0;
		rtvDesc.Texture2DArray.PlaneSlice = 0;

		// 第i个元素
		rtvDesc.Texture2DArray.FirstArraySlice = i;

		// 只有1个数组元素
		rtvDesc.Texture2DArray.ArraySize = 1;

		// 创建RTV
		gD3D12Device->CreateRenderTargetView(mCubeMap.Get(), &rtvDesc, mhCpuRtv[i]);
	}



	// 创建DSV堆
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(gD3D12Device->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));

	mhCubeDSV = CD3DX12_CPU_DESCRIPTOR_HANDLE(mDsvHeap->GetCPUDescriptorHandleForHeapStart(), 0, gDsvDescriptorSize);

	// 创建深度模板视图
	gD3D12Device->CreateDepthStencilView(mCubeDepthStencilBuffer.Get(), nullptr, mhCubeDSV);
}