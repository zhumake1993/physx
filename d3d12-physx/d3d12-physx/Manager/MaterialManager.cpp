#include "MaterialManager.h"

std::unique_ptr<MaterialManager> gMaterialManager = std::make_unique<MaterialManager>();

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

void MaterialManager::Initialize()
{
	for (int i = 0; i < gNumFrameResources; ++i) {
		mFrameResources.push_back(std::make_unique<UploadBuffer<MaterialData>>(gD3D12Device.Get(), mMaterialDataCapacity, false));
	}
}

UINT MaterialManager::GetIndex(const std::string& name)
{
	return mIndices[name];
}

void MaterialManager::AddMaterial(const std::string& name, UINT diffuseMapIndex, UINT normalMapIndex,
	const XMFLOAT4& diffuseAlbedo, const XMFLOAT3& fresnelR0, float roughness,
	const XMFLOAT4X4& matTransform)
{
	MaterialData mat;
	mat.DiffuseAlbedo = diffuseAlbedo;
	mat.FresnelR0 = fresnelR0;
	mat.Roughness = roughness;
	mat.MatTransform = matTransform;
	mat.DiffuseMapIndex = diffuseMapIndex;
	mat.NormalMapIndex = normalMapIndex;

	AddMaterial(name, mat);
}

void MaterialManager::AddMaterial(const std::string& name, const MaterialData& mat)
{
	if (mMaterials.find(name) != mMaterials.end()) {
		OutputMessageBox("Material already exists!");
		return;
	}

	if (mMaterialCount == mMaterialDataCapacity) {
		// 应该进行扩容操作
		// 暂时不实现
		OutputMessageBox("Can not add new material data!");
		return;
	}

	mMaterials[name] = mat;
	mIndices[name] = mMaterialCount;
	mNumFramesDirties[name] = gNumFrameResources;

	++mMaterialCount;
}

void MaterialManager::DeleteMaterial(std::string name)
{
	if (mMaterials.find(name) == mMaterials.end()) {
		OutputMessageBox("Can not find the material!");
		return;
	}

	--mMaterialCount;

	// 应该进行缩容操作
	// 暂时不实现

	mMaterials.erase(name);

	for (auto& p : mIndices) {
		if (p.second > mIndices[name]) {
			--p.second;
			mNumFramesDirties[p.first] = gNumFrameResources;
		}
	}
	mIndices.erase(name);

	mNumFramesDirties.erase(name);
}

MaterialData MaterialManager::GetMaterialData(const std::string& name)
{
	return mMaterials[name];
}

void MaterialManager::SetMaterialData(const std::string& name, MaterialData& mat)
{
	mMaterials[name] = mat;
	mNumFramesDirties[name] = gNumFrameResources;
}

void MaterialManager::UpdateMaterialData()
{
	auto& uploadBuffer = mFrameResources[gCurrFrameResourceIndex];

	for (auto& p : mMaterials) {
		if (mNumFramesDirties[p.first] > 0) {

			XMMATRIX matTransform = XMLoadFloat4x4(&p.second.MatTransform);

			MaterialData matData;
			matData.DiffuseAlbedo = p.second.DiffuseAlbedo;
			matData.FresnelR0 = p.second.FresnelR0;
			matData.Roughness = p.second.Roughness;
			XMStoreFloat4x4(&matData.MatTransform, XMMatrixTranspose(matTransform));
			matData.DiffuseMapIndex = p.second.DiffuseMapIndex;
			matData.NormalMapIndex = p.second.NormalMapIndex;

			uploadBuffer->CopyData(mIndices[p.first], matData);

			--mNumFramesDirties[p.first];
		}
	}
}

ID3D12Resource* MaterialManager::CurrResource() const
{
	return mFrameResources[gCurrFrameResourceIndex]->Resource();
}
