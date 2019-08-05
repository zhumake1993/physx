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

	for (UINT i = 0; i < mMaterialDataCapacity; i++) {
		mIndicesUnused.push_back(i);
	}

	auto DefaultMaterial = std::make_shared<Material>();
	DefaultMaterial->mName = "DefaultMaterial";
	DefaultMaterial->mDiffuseMapIndex = -1;
	DefaultMaterial->mNormalMapIndex = -1;
	DefaultMaterial->mDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DefaultMaterial->mFresnelR0 = XMFLOAT3(0.0f, 0.0f, 0.0f);
	DefaultMaterial->mRoughness = 0.0f;
	AddMaterial(DefaultMaterial);
}

bool MaterialManager::HasMaterial(std::string name)
{
	return mMaterials.find(name) != mMaterials.end();
}

std::shared_ptr<Material> MaterialManager::GetDefaultMaterial()
{
	return mMaterials["DefaultMaterial"];
}

void MaterialManager::AddMaterial(std::shared_ptr<Material> material)
{
	if (mMaterialCount == mMaterialDataCapacity) {
		ThrowMyEx("Can not add new material data!")
	}

	if (material->mName != "") {

		if (HasMaterial(material->mName)) {
			ThrowMyEx("Material already exists!")
		}
	}
	else {

		material->mName = MathHelper::RandStr();
		while (mMaterials.find(material->mName) != mMaterials.end()) {
			material->mName = MathHelper::RandStr();
		}
	}

	material->mIndex = mIndicesUnused.back();
	mIndicesUnused.pop_back();

	mMaterials[material->mName] = material;
	mNumFramesDirties[material->mName] = gNumFrameResources;
	++mMaterialCount;
}

void MaterialManager::SetMaterial(std::shared_ptr<Material> material)
{
	if (!HasMaterial(material->mName)) {
		ThrowMyEx("Material does not exist!")
	}

	mNumFramesDirties[material->mName] = gNumFrameResources;
}

void MaterialManager::DeleteMaterial(const std::string& name)
{
	if (!HasMaterial(name)) {
		ThrowMyEx("Material does not exist!")
	}

	mIndicesUnused.push_back(mMaterials[name]->mIndex);
	mMaterials.erase(name);
	mNumFramesDirties.erase(name);
	--mMaterialCount;
}

void MaterialManager::UpdateMaterialData()
{
	for (auto& p : mMaterials) {
		if (mNumFramesDirties[p.first] > 0) {

			XMMATRIX matTransform = XMLoadFloat4x4(&p.second->mMatTransform);

			MaterialData matData;
			matData.DiffuseAlbedo = p.second->mDiffuseAlbedo;
			matData.FresnelR0 = p.second->mFresnelR0;
			matData.Roughness = p.second->mRoughness;
			XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
			matData.LerpDiffuseAlbedo = p.second->mLerpDiffuseAlbedo;
			matData.DiffuseMapIndex = p.second->mDiffuseMapIndex;
			matData.NormalMapIndex = p.second->mNormalMapIndex;
			matData.LerpPara = p.second->mLerpPara;

			mFrameResource->Copy(p.second->mIndex, matData);

			--mNumFramesDirties[p.first];
		}
	}
}

ID3D12Resource* MaterialManager::CurrResource() const
{
	return mFrameResource->GetCurrResource();
}
