#include "DepthComplexityUseStencil.h"

DepthComplexityUseStencil::DepthComplexityUseStencil()
{
	BuildRootSignature();
	BuildShader();
	BuildPSO();
}

void DepthComplexityUseStencil::Draw(const CD3DX12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv)
{
	//设置视口和剪裁矩形。每次重置指令列表后都要设置视口和剪裁矩形
	gCommandList->RSSetViewports(1, &gScreenViewport);
	gCommandList->RSSetScissorRects(1, &gScissorRect);

	//清空后背缓冲和深度模板缓冲
	gCommandList->ClearRenderTargetView(rtv, DirectX::Colors::Black, 0, nullptr);
	gCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	//设置渲染目标
	gCommandList->OMSetRenderTargets(1, &rtv, true, &dsv);

	// 设置根签名
	gCommandList->SetGraphicsRootSignature(gRootSignatures["main"].Get());

	// 绑定常量缓冲
	auto passCB = gPassCB->GetCurrResource()->Resource();
	gCommandList->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());

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

	// 计算深度复杂度
	gCommandList->SetPipelineState(gPSOs["CountDepthComplexityUseStencil"].Get());
	gInstanceManager->Draw((int)RenderLayer::Opaque);
	gInstanceManager->Draw((int)RenderLayer::OpaqueDynamicReflectors);
	gInstanceManager->Draw((int)RenderLayer::AlphaTested);
	gInstanceManager->Draw((int)RenderLayer::Transparent);

	// 显示深度复杂度（使用模板缓冲）
	gCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	gCommandList->SetPipelineState(gPSOs["ShowDepthComplexityUseStencil"].Get());

	float hasTex[1] = { 0.0f };
	gCommandList->SetGraphicsRoot32BitConstants(0, 1, hasTex, 0);
	float pos[5] = { -1.0f, 1.0f, 0.0f, 2.0f, 2.0f };
	gCommandList->SetGraphicsRoot32BitConstants(0, 5, pos, 1);

	gCommandList->IASetVertexBuffers(0, 1, nullptr);
	gCommandList->IASetIndexBuffer(nullptr);
	gCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 1
	gCommandList->OMSetStencilRef(1);
	float color1[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	gCommandList->SetGraphicsRoot32BitConstants(0, 4, color1, 6);
	gCommandList->DrawInstanced(6, 1, 0, 0);

	// 2
	gCommandList->OMSetStencilRef(2);
	float color2[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
	gCommandList->SetGraphicsRoot32BitConstants(0, 4, color2, 6);
	gCommandList->DrawInstanced(6, 1, 0, 0);

	// 3
	gCommandList->OMSetStencilRef(3);
	float color3[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
	gCommandList->SetGraphicsRoot32BitConstants(0, 4, color3, 6);
	gCommandList->DrawInstanced(6, 1, 0, 0);

	// 4
	gCommandList->OMSetStencilRef(4);
	float color4[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	gCommandList->SetGraphicsRoot32BitConstants(0, 4, color4, 6);
	gCommandList->DrawInstanced(6, 1, 0, 0);

	// 5
	gCommandList->OMSetStencilRef(5);
	float color5[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	gCommandList->SetGraphicsRoot32BitConstants(0, 4, color5, 6);
	gCommandList->DrawInstanced(6, 1, 0, 0);
}

void DepthComplexityUseStencil::BuildRootSignature()
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

void DepthComplexityUseStencil::BuildShader()
{
}

void DepthComplexityUseStencil::BuildPSO()
{
	//
	// 计算深度复杂度
	//
	CD3DX12_BLEND_DESC countDepthComplexityUseStencilBlendState(D3D12_DEFAULT);
	countDepthComplexityUseStencilBlendState.RenderTarget[0].RenderTargetWriteMask = 0;

	D3D12_DEPTH_STENCIL_DESC countDepthComplexityUseStencilDSS;
	countDepthComplexityUseStencilDSS.DepthEnable = false;
	countDepthComplexityUseStencilDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	countDepthComplexityUseStencilDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	countDepthComplexityUseStencilDSS.StencilEnable = true;
	countDepthComplexityUseStencilDSS.StencilReadMask = 0xff;
	countDepthComplexityUseStencilDSS.StencilWriteMask = 0xff;

	countDepthComplexityUseStencilDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_INCR;
	countDepthComplexityUseStencilDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	countDepthComplexityUseStencilDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
	countDepthComplexityUseStencilDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	// 我们不绘制背面多边形，所以这些属性无所谓
	countDepthComplexityUseStencilDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	countDepthComplexityUseStencilDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	countDepthComplexityUseStencilDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	countDepthComplexityUseStencilDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC countDepthComplexityPsoDesc;
	ZeroMemory(&countDepthComplexityPsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	countDepthComplexityPsoDesc.InputLayout = { gInputLayout.data(), (UINT)gInputLayout.size() };
	countDepthComplexityPsoDesc.pRootSignature = gRootSignatures["main"].Get();
	countDepthComplexityPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(gShaders["standardVS"]->GetBufferPointer()),
		gShaders["standardVS"]->GetBufferSize()
	};
	countDepthComplexityPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(gShaders["opaquePS"]->GetBufferPointer()),
		gShaders["opaquePS"]->GetBufferSize()
	};
	countDepthComplexityPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	countDepthComplexityPsoDesc.BlendState = countDepthComplexityUseStencilBlendState;
	countDepthComplexityPsoDesc.DepthStencilState = countDepthComplexityUseStencilDSS;
	countDepthComplexityPsoDesc.SampleMask = UINT_MAX;
	countDepthComplexityPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	countDepthComplexityPsoDesc.NumRenderTargets = 1;
	countDepthComplexityPsoDesc.RTVFormats[0] = gBackBufferFormat;
	countDepthComplexityPsoDesc.SampleDesc.Count = g4xMsaaState ? 4 : 1;
	countDepthComplexityPsoDesc.SampleDesc.Quality = g4xMsaaState ? (g4xMsaaQuality - 1) : 0;
	countDepthComplexityPsoDesc.DSVFormat = gDepthStencilFormat;
	ThrowIfFailed(gD3D12Device->CreateGraphicsPipelineState(&countDepthComplexityPsoDesc, IID_PPV_ARGS(&gPSOs["CountDepthComplexityUseStencil"])));

	//
	// 显示深度复杂度
	//
	D3D12_DEPTH_STENCIL_DESC showDepthComplexityUseStencilDSS;
	showDepthComplexityUseStencilDSS.DepthEnable = false;
	showDepthComplexityUseStencilDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	showDepthComplexityUseStencilDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	showDepthComplexityUseStencilDSS.StencilEnable = true;
	showDepthComplexityUseStencilDSS.StencilReadMask = 0xff;
	showDepthComplexityUseStencilDSS.StencilWriteMask = 0xff;

	showDepthComplexityUseStencilDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	showDepthComplexityUseStencilDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	showDepthComplexityUseStencilDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	showDepthComplexityUseStencilDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// 我们不绘制背面多边形，所以这些属性无所谓
	showDepthComplexityUseStencilDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	showDepthComplexityUseStencilDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	showDepthComplexityUseStencilDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	showDepthComplexityUseStencilDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC showDepthComplexityPsoDesc;
	ZeroMemory(&showDepthComplexityPsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	showDepthComplexityPsoDesc.InputLayout = { gInputLayout.data(), (UINT)gInputLayout.size() };
	showDepthComplexityPsoDesc.pRootSignature = mRootSignature.Get();
	showDepthComplexityPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(gShaders["UIVS"]->GetBufferPointer()),
		gShaders["UIVS"]->GetBufferSize()
	};
	showDepthComplexityPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(gShaders["UIPS"]->GetBufferPointer()),
		gShaders["UIPS"]->GetBufferSize()
	};
	showDepthComplexityPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	showDepthComplexityPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	showDepthComplexityPsoDesc.DepthStencilState = showDepthComplexityUseStencilDSS;
	showDepthComplexityPsoDesc.SampleMask = UINT_MAX;
	showDepthComplexityPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	showDepthComplexityPsoDesc.NumRenderTargets = 1;
	showDepthComplexityPsoDesc.RTVFormats[0] = gBackBufferFormat;
	showDepthComplexityPsoDesc.SampleDesc.Count = g4xMsaaState ? 4 : 1;
	showDepthComplexityPsoDesc.SampleDesc.Quality = g4xMsaaState ? (g4xMsaaQuality - 1) : 0;
	showDepthComplexityPsoDesc.DSVFormat = gDepthStencilFormat;
	ThrowIfFailed(gD3D12Device->CreateGraphicsPipelineState(&showDepthComplexityPsoDesc, IID_PPV_ARGS(&gPSOs["ShowDepthComplexityUseStencil"])));
}