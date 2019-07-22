#pragma once

#include "Common/d3dUtil.h"

class MainRender
{
public:
	MainRender();

	MainRender(const MainRender& rhs) = delete;
	MainRender& operator=(const MainRender& rhs) = delete;
	~MainRender() = default;

	void SetShadow(ID3D12DescriptorHeap* srvDescriptorHeapPtr, CD3DX12_GPU_DESCRIPTOR_HANDLE srv);
	void SetCubeMap(ID3D12DescriptorHeap* srvDescriptorHeapPtr, CD3DX12_GPU_DESCRIPTOR_HANDLE srv);
	void SetSsao(ID3D12DescriptorHeap* srvDescriptorHeapPtr, CD3DX12_GPU_DESCRIPTOR_HANDLE srv);

	void Draw(const CD3DX12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv);

private:
	void BuildRootSignature();
	void BuildShader();
	void BuildPSO();

private:
	ID3D12DescriptorHeap* mShadowSrvDescriptorHeapPtr;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mShadowSrv;

	ID3D12DescriptorHeap* mCubeMapSrvDescriptorHeapPtr;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mCubeMapSrv;

	ID3D12DescriptorHeap* mSsaoSrvDescriptorHeapPtr;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mSsaoSrv;
};