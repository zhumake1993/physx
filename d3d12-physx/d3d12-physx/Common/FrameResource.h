#pragma once

#include "d3dUtil.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
 
// 存储一帧中CPU构建指令列表所需要的资源
struct MainFrameResource
{
public:

	MainFrameResource() = default;
	MainFrameResource(ID3D12Device* device)
	{
		extern const int gNumFrameResources;

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

	void Initialize(ID3D12Device* device)
	{
		extern const int gNumFrameResources;

		mCmdListAllocFrameResources.resize(gNumFrameResources);

		for (int i = 0; i < gNumFrameResources; ++i) {
			ThrowIfFailed(device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(mCmdListAllocFrameResources[i].GetAddressOf())));

			mFenceFrameResources.push_back(0);
		}
	}

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator>& GetCurrCmdListAlloc()
	{
		extern int gCurrFrameResourceIndex;
		return mCmdListAllocFrameResources[gCurrFrameResourceIndex];
	}

	UINT64& GetCurrFence()
	{
		extern int gCurrFrameResourceIndex;
		return mFenceFrameResources[gCurrFrameResourceIndex];
	}

	// 在GPU处理完指令之前，不能够重置指令分配器
	// 因此，每一帧都需要自己的指令分配器
	std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> mCmdListAllocFrameResources; // 帧资源vector

	// 同步
	std::vector<UINT64> mFenceFrameResources; // 帧资源vector
};

template<typename T>
class FrameResource
{
public:

	FrameResource() = default;
	FrameResource(ID3D12Device* device, UINT count, bool isConstantBuffer)
	{
		extern const int gNumFrameResources;

		for (int i = 0; i < gNumFrameResources; ++i) {
			mFrameResources.push_back(std::make_unique<UploadBuffer<T>>(device, count, isConstantBuffer));
		}
	}
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource() {}

	void Initialize(ID3D12Device* device, UINT count, bool isConstantBuffer)
	{
		extern const int gNumFrameResources;

		for (int i = 0; i < gNumFrameResources; ++i) {
			mFrameResources.push_back(std::make_unique<UploadBuffer<T>>(device, count, isConstantBuffer));
		}
	}

	ID3D12Resource* GetCurrResource()
	{
		extern int gCurrFrameResourceIndex;
		return mFrameResources[gCurrFrameResourceIndex]->Resource();
	}

	void Copy(int elementIndex, const T& data)
	{
		extern int gCurrFrameResourceIndex;
		mFrameResources[gCurrFrameResourceIndex]->CopyData(elementIndex, data);
	}

private:
	std::vector<std::unique_ptr<UploadBuffer<T>>> mFrameResources; // 帧资源vector
};