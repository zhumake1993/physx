#pragma once

#include "Common/d3dUtil.h"
#include "Common/FrameResource.h"
#include "Manager/InstanceManager.h"
#include "Manager/TextureManager.h"

struct SsaoConstants
{
	DirectX::XMFLOAT4X4 Proj;
	DirectX::XMFLOAT4X4 InvProj;
	DirectX::XMFLOAT4X4 ProjTex;
	DirectX::XMFLOAT4   OffsetVectors[14];

	// 用于SsaoBlur.hlsl
	DirectX::XMFLOAT4 BlurWeights[3];
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };

	// 遮蔽参数
	float OcclusionRadius = 0.5f;
	float OcclusionFadeStart = 0.2f;
	float OcclusionFadeEnd = 2.0f;
	float SurfaceEpsilon = 0.05f;
};

class Ssao
{
public:
	Ssao(UINT width, UINT height);

	Ssao(const Ssao& rhs) = delete;
	Ssao& operator=(const Ssao& rhs) = delete;
	~Ssao() = default;

	UINT SsaoMapWidth()const;
	UINT SsaoMapHeight()const;

	CD3DX12_GPU_DESCRIPTOR_HANDLE Srv();
	ID3D12DescriptorHeap* GetSrvDescriptorHeapPtr();

	void DrawNormalsAndDepth();

	void UpdateSsaoConstantData(PassConstants& mainPassCB);

	void ComputeSsao(int blurCount);

private:
	// 由于每个像素采样的随机向量数量有限，环境贴图会有噪音，使用模糊可以缓解噪音
	// 使用边界保留模糊，使得模糊不会跨过分界线，从而保留边界
	void BlurAmbientMap(int blurCount);
	void BlurAmbientMap(bool horzBlur);

	void BuildResource();
	void BuildRootSignature();
	void BuildDescriptor();
	void BuildShader();
	void BuildPSO();

	void BuildOffsetVector();
	void BuildRandomVectorTexture();
	std::vector<float> CalcGaussWeights(float sigma);

private:

	ComPtr<ID3D12RootSignature> mRootSignature;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	ComPtr<ID3D12DescriptorHeap> mCbvSrvUavDescriptorHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> mRtvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> mDsvHeap = nullptr;

	UINT mWidth = 0;
	UINT mHeight = 0;
	const DXGI_FORMAT mAmbientMapFormat = DXGI_FORMAT_R16_UNORM;
	const DXGI_FORMAT mNormalMapFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	const int mMaxBlurRadius = 5;
	
	ComPtr<ID3D12Resource> mNormalMap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhNormalMapCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhNormalMapGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhNormalMapCpuRtv;

	ComPtr<ID3D12Resource> mDepthMap;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhDepthMapCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhDepthMapGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhDepthMapCpuDsv;

	ComPtr<ID3D12Resource> mRandomVectorMap;
	ComPtr<ID3D12Resource> mRandomVectorMapUploadBuffer;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhRandomVectorMapCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhRandomVectorMapGpuSrv;

	// 模糊
	ComPtr<ID3D12Resource> mAmbientMap0;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhAmbientMap0CpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhAmbientMap0GpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhAmbientMap0CpuRtv;

	ComPtr<ID3D12Resource> mAmbientMap1;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhAmbientMap1CpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhAmbientMap1GpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhAmbientMap1CpuRtv;

	XMFLOAT4 mOffsets[14];

	std::vector<std::unique_ptr<UploadBuffer<SsaoConstants>>> mFrameResources; // 帧资源vector
};

