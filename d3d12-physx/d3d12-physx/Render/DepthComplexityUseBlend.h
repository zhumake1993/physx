#pragma once

#include "Common/d3dUtil.h"
#include "Common/FrameResource.h"
#include "Manager/InstanceManager.h"
#include "Manager/TextureManager.h"

using Microsoft::WRL::ComPtr;

class DepthComplexityUseBlend
{
public:
	DepthComplexityUseBlend();

	DepthComplexityUseBlend(const DepthComplexityUseBlend& rhs) = delete;
	DepthComplexityUseBlend& operator=(const DepthComplexityUseBlend& rhs) = delete;
	~DepthComplexityUseBlend() = default;

	void Draw(const CD3DX12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv);

private:
	void BuildRootSignature();
	void BuildShader();
	void BuildPSO();

private:
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
};