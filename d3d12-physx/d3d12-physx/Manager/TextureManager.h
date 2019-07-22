#pragma once

#include "Common/d3dUtil.h"

struct Texture
{
	std::string Name;
	std::wstring FileName;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;

	// 纹理数组中的索引
	UINT Index;
};

class TextureManager
{
public:
	TextureManager();
	~TextureManager();

	void Initialize();

	UINT GetIndex(std::string name);
	UINT GetCubeIndex();
	ID3D12DescriptorHeap* GetSrvDescriptorHeapPtr();
	UINT GetMaxNumTextures();

	bool HasTexture(std::string name);
	void AddTextureTex(std::wstring FileName);
	void AddTextureCube(std::wstring FileName);

	void BuildDescriptorHeaps();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuSrvTex();
	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuSrvCube();

private:
	void CreateTexture(std::unique_ptr<Texture> &tex);

public:
	//

private:

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	const UINT mMaxNumTextures = 100;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
	std::unique_ptr<Texture> mCubeMap;
};