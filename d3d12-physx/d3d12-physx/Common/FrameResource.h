#pragma once

#include "d3dUtil.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
 
// �洢һ֡��CPU����ָ���б�����Ҫ����Դ
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
	std::vector<std::unique_ptr<UploadBuffer<T>>> mFrameResources; // ֡��Դvector
};