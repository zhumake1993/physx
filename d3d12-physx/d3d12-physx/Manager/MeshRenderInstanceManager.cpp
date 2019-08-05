#include "Manager/CurrManagers.h"

using namespace DirectX;

MeshRenderInstanceManager::MeshRenderInstanceManager()
{
}

MeshRenderInstanceManager::~MeshRenderInstanceManager()
{
}

void MeshRenderInstanceManager::Initialize()
{
}

std::string MeshRenderInstanceManager::NewMeshRenderName()
{
	auto name = MathHelper::RandStr();
	while (mMeshRenderToParent.find(name) != mMeshRenderToParent.end()) {
		name = MathHelper::RandStr();
	}

	return name;
}

void MeshRenderInstanceManager::AddMeshRender(std::shared_ptr<MeshRender> meshRender)
{
	auto name = meshRender->Name;
	auto world = meshRender->World;
	auto matIndex = meshRender->MaterialIndex;
	auto texTransform = meshRender->TexTransform;
	auto meshName = meshRender->MeshName;
	auto randerLayer = meshRender->RenderLayer;
	auto receiveShadow = meshRender->ReceiveShadow;

	if (randerLayer < 0 || randerLayer >= (int)RenderLayer::Count) {
		ThrowMyEx("RenderLayer illegel!");
	}

	auto& meshRenderInstanceMap = mMeshRenderInstanceLayers[randerLayer];

	if (meshRenderInstanceMap.find(meshName) != meshRenderInstanceMap.end()) {
		meshRenderInstanceMap[meshName]->AddMeshRender(name, world, matIndex, texTransform, receiveShadow);
	}
	else {
		auto meshRenderInstance = std::make_unique<MeshRenderInstance>();
		meshRenderInstance->mMeshName = meshName;
		meshRenderInstance->mMesh = GetCurrMeshManager()->GetMesh(meshName);
		meshRenderInstance->CalculateBoundingBox();

		meshRenderInstance->AddMeshRender(name, world, matIndex, texTransform, receiveShadow);

		meshRenderInstanceMap[meshName] = std::move(meshRenderInstance);
	}

	mMeshRenderToParent[name] = meshRender->Parent;
}

void MeshRenderInstanceManager::DeleteMeshRender(std::shared_ptr<MeshRender> meshRender)
{
	auto name = meshRender->Name;
	auto meshName = meshRender->MeshName;
	auto randerLayer = meshRender->RenderLayer;

	if (mMeshRenderToParent.find(name) == mMeshRenderToParent.end()) {
		ThrowMyEx("meshRender not exist!");
	}

	if (randerLayer < 0 || randerLayer >= (int)RenderLayer::Count) {
		ThrowMyEx("RenderLayer illegel!");
	}

	auto& meshRenderInstanceMap = mMeshRenderInstanceLayers[randerLayer];

	if (meshRenderInstanceMap.find(meshName) == meshRenderInstanceMap.end()) {
		ThrowMyEx("meshName illegel!");
	}

	meshRenderInstanceMap[meshName]->DeleteMeshRender(name);
}

void MeshRenderInstanceManager::UpdateMeshRender(std::shared_ptr<MeshRender> meshRender)
{
	auto name = meshRender->Name;
	auto world = meshRender->World;
	auto matIndex = meshRender->MaterialIndex;
	auto texTransform = meshRender->TexTransform;
	auto meshName = meshRender->MeshName;
	auto randerLayer = meshRender->RenderLayer;
	auto receiveShadow = meshRender->ReceiveShadow;

	if (mMeshRenderToParent.find(name) == mMeshRenderToParent.end()) {
		ThrowMyEx("meshRender not exist!");
	}

	if (randerLayer < 0 || randerLayer >= (int)RenderLayer::Count) {
		ThrowMyEx("RenderLayer illegel!");
	}

	auto& meshRenderInstanceMap = mMeshRenderInstanceLayers[randerLayer];

	if (meshRenderInstanceMap.find(meshName) == meshRenderInstanceMap.end()) {
		ThrowMyEx("meshName illegel!");
	}

	meshRenderInstanceMap[meshName]->UpdateMeshRender(name, world, matIndex, texTransform, receiveShadow);
}

void MeshRenderInstanceManager::UploadMeshRender()
{
	for (auto &layer : mMeshRenderInstanceLayers) {
		for (auto& p : layer) {
			p.second->UploadMeshRender();
		}
	}
}

void MeshRenderInstanceManager::Draw(int randerLayer)
{
	for (auto& p : mMeshRenderInstanceLayers[randerLayer]) {
		p.second->Draw();
	}
}

bool MeshRenderInstanceManager::Pick(FXMVECTOR rayOriginW, FXMVECTOR rayDirW)
{
	bool result = false;

	std::string nameResult;
	float tminResult = MathHelper::Infinity;
	XMVECTOR pointResult;

	for (int i = 0; i < (int)RenderLayer::Count;i++) {

		// ÅÅ³ýÌì¿ÕÇò
		if (i == (int)RenderLayer::Sky)
			continue;

		// ÅÅ³ýÏß¿ò
		if (i == (int)RenderLayer::Wireframe)
			continue;

		for (auto& p : mMeshRenderInstanceLayers[i]) {

			std::string name;
			float tmin;
			XMVECTOR point;

			if (p.second->Pick(rayOriginW, rayDirW, name, tmin, point)) {
				if (tmin < tminResult) {
					result = true;

					nameResult = name;
					tminResult = tmin;
					pointResult = point;
				}
			}
		}
	}

	if (result) {
		XMFLOAT3 hitPoint;
		XMStoreFloat3(&hitPoint, pointResult);

		GetCurrGameObjectManager()->GetGameObject(mMeshRenderToParent[nameResult])->GetPicked(tminResult, hitPoint);
	}

	return result;
}