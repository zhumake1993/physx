#pragma once

#include "Common/d3dUtil.h"

class BlurFilter
{
public:
	BlurFilter(UINT width, UINT height, DXGI_FORMAT format);

	BlurFilter(const BlurFilter& rhs) = delete;
	BlurFilter& operator=(const BlurFilter& rhs) = delete;
	~BlurFilter() = default;

	void CopyIn(ID3D12Resource* input);
	void Execute(int blurCount);
	void CopyOut(ID3D12Resource* output);
	void ExcuteInOut(ID3D12Resource* input, ID3D12Resource* output, int blurCount);

private:
	void BuildResource();
	void BuildRootSignature();
	void BuildDescriptor();
	void BuildShader();
	void BuildPSO();

	std::vector<float> CalcGaussWeights(float sigma);

private:

	const int MaxBlurRadius = 5;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvSrvUavDescriptorHeap = nullptr;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	Microsoft::WRL::ComPtr<ID3D12Resource> mBlurTex0 = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur0CpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur0CpuUav;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur0GpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur0GpuUav;

	Microsoft::WRL::ComPtr<ID3D12Resource> mBlurTex1 = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur1CpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur1CpuUav;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur1GpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur1GpuUav;
};