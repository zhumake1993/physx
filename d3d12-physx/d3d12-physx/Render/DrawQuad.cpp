#include "DrawQuad.h"

DrawQuad::DrawQuad(UINT width, UINT height, DXGI_FORMAT format)
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

void DrawQuad::OnResize(UINT newWidth, UINT newHeight)
{
	if (mWidth != newWidth || mHeight != newHeight) {
		mWidth = newWidth;
		mHeight = newHeight;

		BuildResource();
		BuildDescriptor();
	}
}

void DrawQuad::CopyIn(ID3D12Resource* input)
{
	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_SOURCE));

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mTex.Get(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

	gCommandList->CopyResource(mTex.Get(), input);

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

	gCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mTex.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

void DrawQuad::Draw(const D3D12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv)
{
	//设置视口和剪裁矩形。每次重置指令列表后都要设置视口和剪裁矩形
	gCommandList->RSSetViewports(1, &gScreenViewport);
	gCommandList->RSSetScissorRects(1, &gScissorRect);

	//清空后背缓冲和深度模板缓冲
	gCommandList->ClearRenderTargetView(rtv, DirectX::Colors::Black, 0, nullptr);
	gCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	//设置渲染目标
	gCommandList->OMSetRenderTargets(1, &rtv, true, &dsv);

	// 绑定描述符堆
	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvSrvUavDescriptorHeap.Get() };
	gCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// 设置根签名
	gCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	gCommandList->SetPipelineState(gPSOs["DrawQuad"].Get());

	float hasTex[1] = { 1.0f };
	gCommandList->SetGraphicsRoot32BitConstants(0, 1, hasTex, 0);
	float pos[5] = { -1.0f, 1.0f, 0.0f, 2.0f, 2.0f };
	gCommandList->SetGraphicsRoot32BitConstants(0, 5, pos, 1);
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	gCommandList->SetGraphicsRoot32BitConstants(0, 4, color, 6);

	gCommandList->SetGraphicsRootDescriptorTable(1, mTexGpuSrv);

	// 使用着色器中的SV_VertexID来构建顶点
	gCommandList->IASetVertexBuffers(0, 1, nullptr);
	gCommandList->IASetIndexBuffer(nullptr);
	gCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gCommandList->DrawInstanced(6, 1, 0, 0);
}

void DrawQuad::Draw(ID3D12Resource* input, const D3D12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv)
{
	CopyIn(input);
	Draw(rtv, dsv);
}

void DrawQuad::BuildResource()
{
	// 注意：压缩格式不兼容UAV
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
		IID_PPV_ARGS(&mTex)));
}

void DrawQuad::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE srvTable;
	srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	slotRootParameter[0].InitAsConstants(10, 0);
	slotRootParameter[1].InitAsDescriptorTable(1, &srvTable);

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

void DrawQuad::BuildDescriptor()
{
	// 创建SRV堆
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(gD3D12Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mCbvSrvUavDescriptorHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor(mCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor(mCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	// 保留描述符的引用
	mTexCpuSrv = hCpuDescriptor;
	mTexGpuSrv = hGpuDescriptor;

	// 创建描述符
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = gBackBufferFormat;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	gD3D12Device->CreateShaderResourceView(mTex.Get(), &srvDesc, mTexCpuSrv);
}

void DrawQuad::BuildShader()
{
}

void DrawQuad::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC drawQuadPSODesc;
	ZeroMemory(&drawQuadPSODesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	drawQuadPSODesc.InputLayout = { gInputLayout.data(), (UINT)gInputLayout.size() };
	drawQuadPSODesc.pRootSignature = mRootSignature.Get();
	drawQuadPSODesc.VS =
	{
		reinterpret_cast<BYTE*>(gShaders["UIVS"]->GetBufferPointer()),
		gShaders["UIVS"]->GetBufferSize()
	};
	drawQuadPSODesc.PS =
	{
		reinterpret_cast<BYTE*>(gShaders["UIPS"]->GetBufferPointer()),
		gShaders["UIPS"]->GetBufferSize()
	};
	drawQuadPSODesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	drawQuadPSODesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	drawQuadPSODesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	drawQuadPSODesc.SampleMask = UINT_MAX;
	drawQuadPSODesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	drawQuadPSODesc.NumRenderTargets = 1;
	drawQuadPSODesc.RTVFormats[0] = gBackBufferFormat;
	drawQuadPSODesc.SampleDesc.Count = g4xMsaaState ? 4 : 1;
	drawQuadPSODesc.SampleDesc.Quality = g4xMsaaState ? (g4xMsaaQuality - 1) : 0;
	drawQuadPSODesc.DSVFormat = gDepthStencilFormat;

	// 关闭深度测试
	drawQuadPSODesc.DepthStencilState.DepthEnable = false;
	drawQuadPSODesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	drawQuadPSODesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	ThrowIfFailed(gD3D12Device->CreateGraphicsPipelineState(&drawQuadPSODesc, IID_PPV_ARGS(&gPSOs["DrawQuad"])));
}