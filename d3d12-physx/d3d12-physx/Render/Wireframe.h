#pragma once

#include "Common/d3dUtil.h"

class Wireframe
{
public:
	Wireframe();

	Wireframe(const Wireframe& rhs) = delete;
	Wireframe& operator=(const Wireframe& rhs) = delete;
	~Wireframe() = default;

	void Draw(const CD3DX12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv);

private:
	void BuildRootSignature();
	void BuildShader();
	void BuildPSO();

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
};