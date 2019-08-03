#include "MaterialManager.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

extern Setting gSetting;

extern ComPtr<ID3D12Device> gD3D12Device;
extern ComPtr<ID3D12GraphicsCommandList> gCommandList;

extern const int gNumFrameResources;

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

void MaterialManager::Initialize()
{
	mFrameResource = std::make_unique<FrameResource<MaterialData>>(gD3D12Device.Get(), mMaterialDataCapacity, false);

	auto null = std::make_shared<MaterialData>();
	null->DiffuseMapIndex = -1;
	null->NormalMapIndex = -1;
	null->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	null->FresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	null->Roughness = 0.0f;
	AddMaterial("null", null);
}

bool MaterialManager::HasMaterial(std::string name)
{
	return mMaterials.find(name) != mMaterials.end();
}

std::shared_ptr<MaterialData> MaterialManager::GetMaterial(const std::string name)
{
	if (!HasMaterial(name)) {
		ThrowMyEx("Material does not exist!")
	}

	mNumFramesDirties[name] = gNumFrameResources;

	return mMaterials[name];
}

UINT MaterialManager::GetIndex(const std::string& name)
{
	if (!HasMaterial(name)) {
		ThrowMyEx("Material does not exist!")
	}

	return mIndices[name];
}

void MaterialManager::AddMaterial(const std::string& name, std::shared_ptr<MaterialData> materialData)
{
	if (HasMaterial(name)) {
		ThrowMyEx("Material already exists!")
	}

	if (mMaterialCount == mMaterialDataCapacity) {
		ThrowMyEx("Can not add new material data!")
	}

	mMaterials[name] = materialData;
	mIndices[name] = mMaterialCount++;
	mNumFramesDirties[name] = gNumFrameResources;
}

std::string MaterialManager::AddMaterial(std::shared_ptr<MaterialData> materialData)
{
	auto name = MathHelper::RandStr();
	while (mMaterials.find(name) != mMaterials.end()) {
		name = MathHelper::RandStr();
	}

	AddMaterial(name, materialData);

	return name;
}

void MaterialManager::UpdateMaterialData()
{
	for (auto& p : mMaterials) {
		if (mNumFramesDirties[p.first] > 0) {

			XMMATRIX matTransform = XMLoadFloat4x4(&p.second->MatTransform);

			MaterialData matData;
			matData.DiffuseAlbedo = p.second->DiffuseAlbedo;
			matData.FresnelR0 = p.second->FresnelR0;
			matData.Roughness = p.second->Roughness;
			XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
			matData.LerpDiffuseAlbedo = p.second->LerpDiffuseAlbedo;
			matData.DiffuseMapIndex = p.second->DiffuseMapIndex;
			matData.NormalMapIndex = p.second->NormalMapIndex;
			matData.LerpPara = p.second->LerpPara;

			mFrameResource->Copy(mIndices[p.first], matData);

			--mNumFramesDirties[p.first];
		}
	}
}

ID3D12Resource* MaterialManager::CurrResource() const
{
	return mFrameResource->GetCurrResource();
}
