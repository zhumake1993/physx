#pragma once

#include "Common/d3dUtil.h"

class DrawQuad
{
public:
	DrawQuad(UINT width, UINT height, DXGI_FORMAT format);

	DrawQuad(const DrawQuad& rhs) = delete;
	DrawQuad& operator=(const DrawQuad& rhs) = delete;
	~DrawQuad() = default;

	void OnResize(UINT newWidth, UINT newHeight);

	void CopyIn(ID3D12Resource* input);
	void Draw(const D3D12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv);
	void Draw(ID3D12Resource* input, const D3D12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv);

private:
	void BuildResource();
	void BuildRootSignature();
	void BuildDescriptor();
	void BuildShader();
	void BuildPSO();

private:
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap> mCbvSrvUavDescriptorHeap = nullptr;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	ComPtr<ID3D12Resource> mTex = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mTexCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mTexGpuSrv;
};