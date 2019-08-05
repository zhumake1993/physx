#pragma once

#include "Common/d3dUtil.h"
#include "Common/FrameResource.h"
#include "Manager/MeshRenderInstanceManager.h"
#include "Manager/TextureManager.h"

class ShadowMap
{
public:
	ShadowMap(UINT width, UINT height);

	ShadowMap(const ShadowMap& rhs) = delete;
	ShadowMap& operator=(const ShadowMap& rhs) = delete;
	~ShadowMap() = default;

	void SetBoundingSphere(DirectX::BoundingSphere sceneBounds);

	DirectX::XMFLOAT4X4 GetShadowTransform();

	CD3DX12_GPU_DESCRIPTOR_HANDLE Srv();
	ID3D12DescriptorHeap* GetSrvDescriptorHeapPtr();

	void Update(DirectX::XMFLOAT3 mRotatedLightDirection);

	void DrawSceneToShadowMap();

private:
	void BuildResource();
	void BuildRootSignature();
	void BuildDescriptor();
	void BuildShader();
	void BuildPSO();

private:

	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCbvSrvUavDescriptorHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap = nullptr;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R24G8_TYPELESS;

	DirectX::BoundingSphere mSceneBounds;
	DirectX::XMFLOAT4X4 mShadowTransform = MathHelper::Identity4x4();

	Microsoft::WRL::ComPtr<ID3D12Resource> mShadowMap = nullptr;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mhGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mhCpuDsv;

	std::unique_ptr<FrameResource<PassConstants>> mFrameResource; // ֡��Դ
};

