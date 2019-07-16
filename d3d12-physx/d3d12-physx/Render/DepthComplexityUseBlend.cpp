#include "DepthComplexityUseBlend.h"

DepthComplexityUseBlend::DepthComplexityUseBlend()
{
	BuildRootSignature();
	BuildShader();
	BuildPSO();
}

void DepthComplexityUseBlend::Draw(const CD3DX12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv)
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
	gCommandList->SetGraphicsRootSignature(mRootSignature.Get());

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

	gCommandList->SetPipelineState(gPSOs["ShowDepthComplexityUseBlend"].Get());
	gInstanceManager->Draw((int)RenderLayer::Opaque);
	gInstanceManager->Draw((int)RenderLayer::OpaqueDynamicReflectors);
	gInstanceManager->Draw((int)RenderLayer::AlphaTested);
	gInstanceManager->Draw((int)RenderLayer::Transparent);
}

void DepthComplexityUseBlend::BuildRootSignature()
{
	mRootSignature = gRootSignatures["main"];
}

void DepthComplexityUseBlend::BuildShader()
{
	gShaders["GreyVS"] = d3dUtil::CompileShader(L"Shaders\\Grey.hlsl", nullptr, "VS", "vs_5_1");
	gShaders["GreyPS"] = d3dUtil::CompileShader(L"Shaders\\Grey.hlsl", nullptr, "PS", "ps_5_1");
}

void DepthComplexityUseBlend::BuildPSO()
{
	D3D12_RENDER_TARGET_BLEND_DESC showDepthComplexityUseBlendBlendDesc;
	showDepthComplexityUseBlendBlendDesc.BlendEnable = true;
	showDepthComplexityUseBlendBlendDesc.LogicOpEnable = false;
	showDepthComplexityUseBlendBlendDesc.SrcBlend = D3D12_BLEND_ONE;
	showDepthComplexityUseBlendBlendDesc.DestBlend = D3D12_BLEND_ONE;
	showDepthComplexityUseBlendBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	showDepthComplexityUseBlendBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	showDepthComplexityUseBlendBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	showDepthComplexityUseBlendBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	showDepthComplexityUseBlendBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	showDepthComplexityUseBlendBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_DEPTH_STENCIL_DESC showDepthComplexityUseBlendDSS;
	showDepthComplexityUseBlendDSS.DepthEnable = false;
	showDepthComplexityUseBlendDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	showDepthComplexityUseBlendDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	showDepthComplexityUseBlendDSS.StencilEnable = false;
	showDepthComplexityUseBlendDSS.StencilReadMask = 0xff;
	showDepthComplexityUseBlendDSS.StencilWriteMask = 0xff;

	showDepthComplexityUseBlendDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	showDepthComplexityUseBlendDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	showDepthComplexityUseBlendDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	showDepthComplexityUseBlendDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

	// 我们不绘制背面多边形，所以这些属性无所谓
	showDepthComplexityUseBlendDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	showDepthComplexityUseBlendDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	showDepthComplexityUseBlendDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	showDepthComplexityUseBlendDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC showDepthComplexityUseBlendPsoDesc;
	ZeroMemory(&showDepthComplexityUseBlendPsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	showDepthComplexityUseBlendPsoDesc.InputLayout = { gInputLayout.data(), (UINT)gInputLayout.size() };
	showDepthComplexityUseBlendPsoDesc.pRootSignature = mRootSignature.Get();
	showDepthComplexityUseBlendPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(gShaders["GreyVS"]->GetBufferPointer()),
		gShaders["GreyVS"]->GetBufferSize()
	};
	showDepthComplexityUseBlendPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(gShaders["GreyPS"]->GetBufferPointer()),
		gShaders["GreyPS"]->GetBufferSize()
	};
	showDepthComplexityUseBlendPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	showDepthComplexityUseBlendPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	showDepthComplexityUseBlendPsoDesc.BlendState.RenderTarget[0] = showDepthComplexityUseBlendBlendDesc;
	showDepthComplexityUseBlendPsoDesc.DepthStencilState = showDepthComplexityUseBlendDSS;
	showDepthComplexityUseBlendPsoDesc.SampleMask = UINT_MAX;
	showDepthComplexityUseBlendPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	showDepthComplexityUseBlendPsoDesc.NumRenderTargets = 1;
	showDepthComplexityUseBlendPsoDesc.RTVFormats[0] = gBackBufferFormat;
	showDepthComplexityUseBlendPsoDesc.SampleDesc.Count = g4xMsaaState ? 4 : 1;
	showDepthComplexityUseBlendPsoDesc.SampleDesc.Quality = g4xMsaaState ? (g4xMsaaQuality - 1) : 0;
	showDepthComplexityUseBlendPsoDesc.DSVFormat = gDepthStencilFormat;
	ThrowIfFailed(gD3D12Device->CreateGraphicsPipelineState(&showDepthComplexityUseBlendPsoDesc, IID_PPV_ARGS(&gPSOs["ShowDepthComplexityUseBlend"])));
}