#pragma once

#include "d3dUtil.h"
#include "MathHelper.h"
#include "UploadBuffer.h"

struct PassConstants
{
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProjTex = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ShadowTransform = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	DirectX::XMFLOAT4 FogColor = { 0.7f, 0.7f, 0.7f, 1.0f };
	float gFogStart = 5.0f;
	float gFogRange = 150.0f;
	DirectX::XMFLOAT2 cbPerObjectPad2;

	// ���� [0, NUM_DIR_LIGHTS) ��ƽ�й�
	// ���� [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) �ǵ��
	// ���� [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS) �Ǿ۹�
	Light Lights[MaxLights];
};
 
// �洢һ֡��CPU����ָ���б�����Ҫ����Դ
struct MainFrameResource
{
public:

	MainFrameResource(ID3D12Device* device)
	{
		mCmdListAllocFrameResources.resize(gNumFrameResources);

		for (int i = 0; i < gNumFrameResources; ++i) {
			ThrowIfFailed(device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(mCmdListAllocFrameResources[i].GetAddressOf())));

			mFenceFrameResources.push_back(0);
		}
	}
	MainFrameResource(const MainFrameResource& rhs) = delete;
	MainFrameResource& operator=(const MainFrameResource& rhs) = delete;
	~MainFrameResource() {}

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& GetCurrCmdListAlloc()
	{
		return mCmdListAllocFrameResources[gCurrFrameResourceIndex];
	}

	UINT64& GetCurrFence()
	{
		return mFenceFrameResources[gCurrFrameResourceIndex];
	}

	// ��GPU������ָ��֮ǰ�����ܹ�����ָ�������
	// ��ˣ�ÿһ֡����Ҫ�Լ���ָ�������
	std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> mCmdListAllocFrameResources; // ֡��Դvector

	// ͬ��
	std::vector<UINT64> mFenceFrameResources; // ֡��Դvector
};

template<typename T>
class FrameResource
{
public:

	FrameResource() = default;
	FrameResource(ID3D12Device* device, UINT count, bool isConstantBuffer)
	{
		for (int i = 0; i < gNumFrameResources; ++i) {
			mFrameResources.push_back(std::make_unique<UploadBuffer<T>>(device, count, isConstantBuffer));
		}
	}
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource() {}

	void Initialize(ID3D12Device* device, UINT count, bool isConstantBuffer)
	{
		for (int i = 0; i < gNumFrameResources; ++i) {
			mFrameResources.push_back(std::make_unique<UploadBuffer<T>>(device, count, isConstantBuffer));
		}
	}

	std::unique_ptr<UploadBuffer<T>>& GetCurrResource()
	{
		return mFrameResources[gCurrFrameResourceIndex];
	}

	void Copy(int elementIndex, const T& data)
	{
		mFrameResources[gCurrFrameResourceIndex]->CopyData(elementIndex, data);
	}

private:
	std::vector<std::unique_ptr<UploadBuffer<T>>> mFrameResources; // ֡��Դvector
};

extern std::unique_ptr<FrameResource<PassConstants>> gPassCB;