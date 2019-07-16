#include "MainRender.h"

MainRender::MainRender()
{
	BuildRootSignature();
	BuildShader();
	BuildPSO();
}

void MainRender::SetShadow(ID3D12DescriptorHeap* srvDescriptorHeapPtr, CD3DX12_GPU_DESCRIPTOR_HANDLE srv)
{
	mShadowSrvDescriptorHeapPtr = srvDescriptorHeapPtr;
	mShadowSrv = srv;
}

void MainRender::SetCubeMap(ID3D12DescriptorHeap* srvDescriptorHeapPtr, CD3DX12_GPU_DESCRIPTOR_HANDLE srv)
{
	mCubeMapSrvDescriptorHeapPtr = srvDescriptorHeapPtr;
	mCubeMapSrv = srv;
}

void MainRender::SetSsao(ID3D12DescriptorHeap* srvDescriptorHeapPtr, CD3DX12_GPU_DESCRIPTOR_HANDLE srv)
{
	mSsaoSrvDescriptorHeapPtr = srvDescriptorHeapPtr;
	mSsaoSrv = srv;
}

void MainRender::Draw(const CD3DX12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv)
{
	//�����ӿںͼ��þ��Ρ�ÿ������ָ���б��Ҫ�����ӿںͼ��þ���
	gCommandList->RSSetViewports(1, &gScreenViewport);
	gCommandList->RSSetScissorRects(1, &gScissorRect);

	//��պ󱳻�������ģ�建��
	gCommandList->ClearRenderTargetView(rtv, DirectX::Colors::Black, 0, nullptr);
	gCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	//������ȾĿ��
	gCommandList->OMSetRenderTargets(1, &rtv, true, &dsv);

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

	// ����Ӱ��ͼ
	ID3D12DescriptorHeap* descriptorHeapsShadow[] = { mShadowSrvDescriptorHeapPtr };
	gCommandList->SetDescriptorHeaps(_countof(descriptorHeapsShadow), descriptorHeapsShadow);
	gCommandList->SetGraphicsRootDescriptorTable(5, mShadowSrv);

	// ��Ssao
	ID3D12DescriptorHeap* descriptorHeapsSsao[] = { mSsaoSrvDescriptorHeapPtr };
	gCommandList->SetDescriptorHeaps(_countof(descriptorHeapsSsao), descriptorHeapsSsao);
	gCommandList->SetGraphicsRootDescriptorTable(6, mSsaoSrv);

	gCommandList->SetPipelineState(gPSOs["opaque"].Get());
	gInstanceManager->Draw((int)RenderLayer::Opaque);

	gCommandList->SetPipelineState(gPSOs["alphaTested"].Get());
	gInstanceManager->Draw((int)RenderLayer::AlphaTested);

	gCommandList->SetPipelineState(gPSOs["sky"].Get());
	gInstanceManager->Draw((int)RenderLayer::Sky);

	gCommandList->SetPipelineState(gPSOs["transparent"].Get());
	gInstanceManager->Draw((int)RenderLayer::Transparent);

	// ʹ�ö�̬��������ͼ���ƶ�̬��������
	// �󶨶�̬��������ͼ����������
	ID3D12DescriptorHeap* descriptorHeapsCube[] = { mCubeMapSrvDescriptorHeapPtr };
	gCommandList->SetDescriptorHeaps(_countof(descriptorHeapsCube), descriptorHeapsCube);
	gCommandList->SetGraphicsRootDescriptorTable(4, mCubeMapSrv);

	gCommandList->SetPipelineState(gPSOs["opaque"].Get());
	gInstanceManager->Draw((int)RenderLayer::OpaqueDynamicReflectors);
}

void MainRender::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, gTextureManager->GetMaxNumTextures(), 3, 0);

	CD3DX12_DESCRIPTOR_RANGE texCubeMap;
	texCubeMap.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);

	CD3DX12_DESCRIPTOR_RANGE texShadowMap;
	texShadowMap.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE texSsaoMap;
	texSsaoMap.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0);

	CD3DX12_ROOT_PARAMETER slotRootParameter[7];

	slotRootParameter[0].InitAsShaderResourceView(0, 1); // �ṹ������InstanceData
	slotRootParameter[1].InitAsConstantBufferView(1); // ��������PassConstants
	slotRootParameter[2].InitAsShaderResourceView(1, 1); // �ṹ������MaterialData
	slotRootParameter[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL); // ����
	slotRootParameter[4].InitAsDescriptorTable(1, &texCubeMap, D3D12_SHADER_VISIBILITY_PIXEL); // ��������ͼ
	slotRootParameter[5].InitAsDescriptorTable(1, &texShadowMap, D3D12_SHADER_VISIBILITY_PIXEL); // ��Ӱ��ͼ
	slotRootParameter[6].InitAsDescriptorTable(1, &texSsaoMap, D3D12_SHADER_VISIBILITY_PIXEL); // Ssao��ͼ

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
		IID_PPV_ARGS(gRootSignatures["main"].GetAddressOf())));
}

void MainRender::BuildShader()
{
	const D3D_SHADER_MACRO defines[] =
	{
		"FOG", "1",
		NULL, NULL
	};

	const D3D_SHADER_MACRO alphaTestDefines[] =
	{
		"FOG", "1",
		"ALPHA_TEST", "1",
		NULL, NULL
	};

	gShaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	gShaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", defines, "PS", "ps_5_1");
	gShaders["alphaTestedPS"] = d3dUtil::CompileShader(L"Shaders\\Default.hlsl", alphaTestDefines, "PS", "ps_5_1");

	gShaders["UIVS"] = d3dUtil::CompileShader(L"Shaders\\UI.hlsl", nullptr, "VS", "vs_5_1");
	gShaders["UIPS"] = d3dUtil::CompileShader(L"Shaders\\UI.hlsl", nullptr, "PS", "ps_5_1");

	gShaders["skyVS"] = d3dUtil::CompileShader(L"Shaders\\Sky.hlsl", nullptr, "VS", "vs_5_1");
	gShaders["skyPS"] = d3dUtil::CompileShader(L"Shaders\\Sky.hlsl", nullptr, "PS", "ps_5_1");
}

void MainRender::BuildPSO()
{
	//
	// ��͸�������PSO
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { gInputLayout.data(), (UINT)gInputLayout.size() };
	opaquePsoDesc.pRootSignature = gRootSignatures["main"].Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(gShaders["standardVS"]->GetBufferPointer()),
		gShaders["standardVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(gShaders["opaquePS"]->GetBufferPointer()),
		gShaders["opaquePS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = gBackBufferFormat;
	opaquePsoDesc.SampleDesc.Count = g4xMsaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = g4xMsaaState ? (g4xMsaaQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = gDepthStencilFormat;
	ThrowIfFailed(gD3D12Device->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&gPSOs["opaque"])));

	//
	// ͸������
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPsoDesc = opaquePsoDesc;

	D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
	transparencyBlendDesc.BlendEnable = true;
	transparencyBlendDesc.LogicOpEnable = false;
	transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	transparentPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
	ThrowIfFailed(gD3D12Device->CreateGraphicsPipelineState(&transparentPsoDesc, IID_PPV_ARGS(&gPSOs["transparent"])));

	//
	// alpha��������
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC alphaTestedPsoDesc = opaquePsoDesc;
	alphaTestedPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(gShaders["alphaTestedPS"]->GetBufferPointer()),
		gShaders["alphaTestedPS"]->GetBufferSize()
	};
	alphaTestedPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	ThrowIfFailed(gD3D12Device->CreateGraphicsPipelineState(&alphaTestedPsoDesc, IID_PPV_ARGS(&gPSOs["alphaTested"])));

	//
	// �����
	//
	D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDesc = opaquePsoDesc;

	// �������������ڲ�����˹رձ����޳�
	skyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// ����Ⱥ�����LESS��ΪLESS_EQUAL
	// ���������Ȼ��屻���Ϊ1����ô���ֵz = 1������򽫲���ͨ����Ȳ���
	skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	skyPsoDesc.pRootSignature = gRootSignatures["main"].Get();
	skyPsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(gShaders["skyVS"]->GetBufferPointer()),
		gShaders["skyVS"]->GetBufferSize()
	};
	skyPsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(gShaders["skyPS"]->GetBufferPointer()),
		gShaders["skyPS"]->GetBufferSize()
	};
	ThrowIfFailed(gD3D12Device->CreateGraphicsPipelineState(&skyPsoDesc, IID_PPV_ARGS(&gPSOs["sky"])));
}