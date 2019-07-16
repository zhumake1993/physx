#pragma once

#include "Common/d3dUtil.h"

class MultiplyFilter
{
public:
	MultiplyFilter(UINT width, UINT height, DXGI_FORMAT format);

	MultiplyFilter(const MultiplyFilter& rhs) = delete;
	MultiplyFilter& operator=(const MultiplyFilter& rhs) = delete;
	~MultiplyFilter() = default;

	void CopyIn0(ID3D12Resource* input);
	void CopyIn1(ID3D12Resource* input);
	void Execute();
	void CopyOut(ID3D12Resource* output);
	void ExcuteInOut(ID3D12Resource* input0, ID3D12Resource* input1, ID3D12Resource* output);

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

	Microsoft::WRL::ComPtr<ID3D12Resource> mInput0 = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mInput0CpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mInput0GpuSrv;

	Microsoft::WRL::ComPtr<ID3D12Resource> mInput1 = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mInput1CpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mInput1GpuSrv;

	Microsoft::WRL::ComPtr<ID3D12Resource> mOutput = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mOutputCpuUav;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mOutputGpuUav;
};