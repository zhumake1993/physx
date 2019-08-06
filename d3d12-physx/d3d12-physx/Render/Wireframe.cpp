#include "Wireframe.h"
#include "Manager/SceneManager.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

extern Setting gSetting;

extern ComPtr<ID3D12Device> gD3D12Device;
extern ComPtr<ID3D12GraphicsCommandList> gCommandList;

#include "Common/FrameResource.h"
extern std::unique_ptr<FrameResource<PassConstants>> gPassCB;

extern std::vector<D3D12_INPUT_ELEMENT_DESC> gInputLayout;
extern std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> gRootSignatures;
extern std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> gShaders;
extern std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> gPSOs;

Wireframe::Wireframe()
{
	BuildRootSignature();
	BuildShader();
	BuildPSO();
}

void Wireframe::Draw(const CD3DX12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv)
{
	//�����ӿںͼ��þ��Ρ�ÿ������ָ���б��Ҫ�����ӿںͼ��þ���
	gCommandList->RSSetViewports(1, &gSetting.ScreenViewport);
	gCommandList->RSSetScissorRects(1, &gSetting.ScissorRect);

	//��պ󱳻�������ģ�建��
	gCommandList->ClearRenderTargetView(rtv, DirectX::Colors::Black, 0, nullptr);
	gCommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	//������ȾĿ��
	gCommandList->OMSetRenderTargets(1, &rtv, true, &dsv);

	// ���ø�ǩ��
	gCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	// �󶨳�������
	auto passCB = gPassCB->GetCurrResource();
	gCommandList->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());

	// �����в��ʡ����ڽṹ�����壬���ǿ����ƹ��ѣ�ʹ�ø�������
	auto matBuffer = GetCurrMaterialManager()->CurrResource();
	gCommandList->SetGraphicsRootShaderResourceView(2, matBuffer->GetGPUVirtualAddress());

	// ����������
	ID3D12DescriptorHeap* descriptorHeaps[] = { GetCurrTextureManager()->GetSrvDescriptorHeapPtr() };
	gCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	// �����е�����
	gCommandList->SetGraphicsRootDescriptorTable(3, GetCurrTextureManager()->GetGpuSrvTex());

	// ���������������ͼ
	gCommandList->SetGraphicsRootDescriptorTable(4, GetCurrTextureManager()->GetGpuSrvCube());

	gCommandList->SetPipelineState(gPSOs["Wireframe"].Get());
	GetCurrMeshRenderInstanceManager()->Draw((int)RenderLayer::Opaque);
	GetCurrMeshRenderInstanceManager()->Draw((int)RenderLayer::OpaqueDynamicReflectors);
	GetCurrMeshRenderInstanceManager()->Draw((int)RenderLayer::AlphaTested);
	GetCurrMeshRenderInstanceManager()->Draw((int)RenderLayer::Transparent);
}

void Wireframe::BuildRootSignature()
{
	mRootSignature = gRootSignatures["main"];
}

void Wireframe::BuildShader()
{
	gShaders["WireframeVS"] = d3dUtil::CompileShader(L"Shaders\\Wireframe.hlsl", nullptr, "VS", "vs_5_1");
	gShaders["WireframePS"] = d3dUtil::CompileShader(L"Shaders\\Wireframe.hlsl", nullptr, "PS", "ps_5_1");
}

void Wireframe::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc;
	ZeroMemory(&opaqueWireframePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaqueWireframePsoDesc.InputLayout = { gInputLayout.data(), (UINT)gInputLayout.size() };
	opaqueWireframePsoDesc.pRootSignature = mRootSignature.Get();
	opaqueWireframePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(gShaders["WireframeVS"]->GetBufferPointer()),
		gShaders["WireframeVS"]->GetBufferSize()
	};
	opaqueWireframePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(gShaders["WireframePS"]->GetBufferPointer()),
		gShaders["WireframePS"]->GetBufferSize()
	};
	opaqueWireframePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	opaqueWireframePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	opaqueWireframePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaqueWireframePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaqueWireframePsoDesc.SampleMask = UINT_MAX;
	opaqueWireframePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaqueWireframePsoDesc.NumRenderTargets = 1;
	opaqueWireframePsoDesc.RTVFormats[0] = gSetting.BackBufferFormat;
	opaqueWireframePsoDesc.SampleDesc.Count = gSetting.X4MsaaState ? 4 : 1;
	opaqueWireframePsoDesc.SampleDesc.Quality = gSetting.X4MsaaState ? (gSetting.X4MsaaQuality - 1) : 0;
	opaqueWireframePsoDesc.DSVFormat = gSetting.DepthStencilFormat;
	ThrowIfFailed(gD3D12Device->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&gPSOs["Wireframe"])));
}