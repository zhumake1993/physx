#pragma once

#include "Common/d3dUtil.h"
#include "Common/FrameResource.h"
#include "Manager/InstanceManager.h"
#include "Manager/TextureManager.h"

enum class CubeMapFace : int
{
	PositiveX = 0,
	NegativeX = 1,
	PositiveY = 2,
	NegativeY = 3,
	PositiveZ = 4,
	NegativeZ = 5
};

class CubeMap
{
public:
	CubeMap(DXGI_FORMAT format, DXGI_FORMAT depthStencilFormat);

	CubeMap(const CubeMap& rhs) = delete;
	CubeMap& operator=(const CubeMap& rhs) = delete;
	~CubeMap() = default;

	void SetShadow(ID3D12DescriptorHeap* srvDescriptorHeapPtr, CD3DX12_GPU_DESCRIPTOR_HANDLE srv);
	void SetSsao(ID3D12DescriptorHeap* srvDescriptorHeapPtr, CD3DX12_GPU_DESCRIPTOR_HANDLE srv);

	CD3DX12_GPU_DESCRIPTOR_HANDLE Srv();
	ID3D12DescriptorHeap* GetSrvDescriptorHeapPtr();

	void BuildCubeFaceCamera(float x, float y, float z);

	void UpdatePassConstantsData(PassConstants& mainPassCB);

	void DrawSceneToCubeMap();

private:
	void BuildResource();
	void BuildDescriptor();

private:
	const UINT mCubeMapSize = 512;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	ComPtr<ID3D12DescriptorHeap> mCbvSrvUavDescriptorHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> mRtvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> mDsvHeap = nullptr;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	ComPtr<ID3D12Resource> mCubeMap = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuRtv[6];

	ComPtr<ID3D12Resource> mCubeDepthStencilBuffer = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCubeDSV;

	Camera mCubeMapCamera[6];

	std::vector<std::unique_ptr<UploadBuffer<PassConstants>>> mFrameResources; // Ö¡×ÊÔ´vector

	ID3D12DescriptorHeap* mShadowSrvDescriptorHeapPtr;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mShadowSrv;

	ID3D12DescriptorHeap* mSsaoSrvDescriptorHeapPtr;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mSsaoSrv;
};