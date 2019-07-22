#pragma once

#include "Common/d3dUtil.h"

using Microsoft::WRL::ComPtr;

class DepthComplexityUseStencil
{
public:
	DepthComplexityUseStencil();

	DepthComplexityUseStencil(const DepthComplexityUseStencil& rhs) = delete;
	DepthComplexityUseStencil& operator=(const DepthComplexityUseStencil& rhs) = delete;
	~DepthComplexityUseStencil() = default;

	void Draw(const CD3DX12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv);

private:
	void BuildRootSignature();
	void BuildShader();
	void BuildPSO();

private:
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
};