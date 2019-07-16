#include "TextureManager.h"

std::unique_ptr<TextureManager> gTextureManager = std::make_unique<TextureManager>();

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

void TextureManager::Initialize()
{
}

UINT TextureManager::GetIndex(std::string name)
{
	return mTextures[name]->Index;
}

UINT TextureManager::GetCubeIndex()
{
	return mCubeMap->Index;
}

ID3D12DescriptorHeap* TextureManager::GetSrvDescriptorHeapPtr()
{
	return mSrvDescriptorHeap.Get();
}

UINT TextureManager::GetMaxNumTextures()
{
	return mMaxNumTextures;
}

void TextureManager::AddTextureTex(std::wstring FileName)
{
	auto tex = std::make_unique<Texture>();
	tex->FileName = FileName;

	CreateTexture(tex);

	if(tex->Name != "")
		mTextures[tex->Name] = std::move(tex);
}

void TextureManager::AddTextureCube(std::wstring FileName)
{
	auto tex = std::make_unique<Texture>();
	tex->FileName = FileName;

	CreateTexture(tex);

	if (tex->Name != "")
		mCubeMap = std::move(tex);
}

void TextureManager::BuildDescriptorHeaps()
{
	// 创建描述符堆
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = mMaxNumTextures;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(gD3D12Device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mSrvDescriptorHeap)));

	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// 创建天空球的srv
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = mCubeMap->Resource->GetDesc().MipLevels;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Format = mCubeMap->Resource->GetDesc().Format;
	gD3D12Device->CreateShaderResourceView(mCubeMap->Resource.Get(), &srvDesc, hDescriptor);
	mCubeMap->Index = 0;
	hDescriptor.Offset(1, gCbvSrvUavDescriptorSize);

	// 创建纹理的srv
	UINT currIndex = 0;
	srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	for (const auto& p : mTextures) {
		srvDesc.Format = p.second->Resource->GetDesc().Format;
		srvDesc.Texture2D.MipLevels = p.second->Resource->GetDesc().MipLevels;
		gD3D12Device->CreateShaderResourceView(p.second->Resource.Get(), &srvDesc, hDescriptor);

		p.second->Index = currIndex++;
		hDescriptor.Offset(1, gCbvSrvUavDescriptorSize);
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGpuSrvTex()
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv(mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	hGpuSrv.Offset(1, gCbvSrvUavDescriptorSize);
	return hGpuSrv;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGpuSrvCube()
{
	return mSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

void TextureManager::CreateTexture(std::unique_ptr<Texture> &tex)
{
	auto fileNameString = WStringToString(tex->FileName);
	auto begin = fileNameString.find_last_of('/');
	auto end = fileNameString.find_last_of('.');
	tex->Name = fileNameString.substr(begin + 1, end - begin - 1);

	if (mTextures.find(tex->Name) != mTextures.end()) {
		OutputMessageBox("Texture already exists!");
		tex->Name = "";
		return;
	}

	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(gD3D12Device.Get(),
		gCommandList.Get(), tex->FileName.c_str(),
		tex->Resource, tex->UploadHeap));
}
