#pragma once

#include "Common/d3dUtil.h"

class SobelFilter
{
public:
	SobelFilter(UINT width, UINT height, DXGI_FORMAT format);
		
	SobelFilter(const SobelFilter& rhs)=delete;
	SobelFilter& operator=(const SobelFilter& rhs)=delete;
	~SobelFilter()=default;

	void CopyIn(ID3D12Resource* input);
	void Execute();
	void CopyOut(ID3D12Resource* output);
	void ExcuteInOut(ID3D12Resource* input, ID3D12Resource* output);

private:
	void BuildResource();
	void BuildRootSignature();
	void BuildDescriptor();
	void BuildShader();
	void BuildPSO();

private:

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvSrvUavDescriptorHeap = nullptr;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	Microsoft::WRL::ComPtr<ID3D12Resource> mInput = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mInputCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mInputGpuSrv;

	Microsoft::WRL::ComPtr<ID3D12Resource> mOutput = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mOutputCpuUav;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mOutputGpuUav;
};

 