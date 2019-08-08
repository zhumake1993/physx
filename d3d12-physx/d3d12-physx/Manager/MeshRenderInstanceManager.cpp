#include "Manager/MeshRenderInstanceManager.h"
#include "Manager/SceneManager.h"

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

void MeshRenderInstanceManager::AddMeshRender(MeshRenderCPT* meshRenderCPT)
{
	if (meshRenderCPT->mRenderLayer < 0 || meshRenderCPT->mRenderLayer >= (int)RenderLayer::Count) {
		ThrowMyEx("RenderLayer illegel!");
	}

	auto& meshRenderInstanceMap = mMeshRenderInstanceLayers[meshRenderCPT->mRenderLayer];

	if (meshRenderInstanceMap.find(meshRenderCPT->mMeshName) != meshRenderInstanceMap.end()) {
		meshRenderInstanceMap[meshRenderCPT->mMeshName]->AddMeshRender(meshRenderCPT->mName, meshRenderCPT->GetWorld(), meshRenderCPT->mMaterial->mIndex, meshRenderCPT->mTexTransform, meshRenderCPT->mReceiveShadow);
	}
	else {
		auto meshRenderInstance = std::make_unique<MeshRenderInstance>();
		meshRenderInstance->mMeshName = meshRenderCPT->mMeshName;
		meshRenderInstance->mMesh = GetCurrMeshManager()->GetMesh(meshRenderCPT->mMeshName);
		meshRenderInstance->CalculateBoundingBox();

		meshRenderInstance->AddMeshRender(meshRenderCPT->mName, meshRenderCPT->GetWorld(), meshRenderCPT->mMaterial->mIndex, meshRenderCPT->mTexTransform, meshRenderCPT->mReceiveShadow);

		meshRenderInstanceMap[meshRenderCPT->mMeshName] = std::move(meshRenderInstance);
	}

	mMeshRenderToParent[meshRenderCPT->mName] = meshRenderCPT->mParent;
}

void MeshRenderInstanceManager::DeleteMeshRender(MeshRenderCPT* meshRenderCPT)
{
	if (mMeshRenderToParent.find(meshRenderCPT->mName) == mMeshRenderToParent.end()) {
		ThrowMyEx("meshRender not exist!");
	}

	if (meshRenderCPT->mRenderLayer < 0 || meshRenderCPT->mRenderLayer >= (int)RenderLayer::Count) {
		ThrowMyEx("RenderLayer illegel!");
	}

	auto& meshRenderInstanceMap = mMeshRenderInstanceLayers[meshRenderCPT->mRenderLayer];

	if (meshRenderInstanceMap.find(meshRenderCPT->mMeshName) == meshRenderInstanceMap.end()) {
		ThrowMyEx("meshName illegel!");
	}

	meshRenderInstanceMap[meshRenderCPT->mMeshName]->DeleteMeshRender(meshRenderCPT->mName);
}

void MeshRenderInstanceManager::UpdateMeshRender(MeshRenderCPT* meshRenderCPT)
{
	if (mMeshRenderToParent.find(meshRenderCPT->mName) == mMeshRenderToParent.end()) {
		ThrowMyEx("meshRender not exist!");
	}

	if (meshRenderCPT->mRenderLayer < 0 || meshRenderCPT->mRenderLayer >= (int)RenderLayer::Count) {
		ThrowMyEx("RenderLayer illegel!");
	}

	auto& meshRenderInstanceMap = mMeshRenderInstanceLayers[meshRenderCPT->mRenderLayer];

	if (meshRenderInstanceMap.find(meshRenderCPT->mMeshName) == meshRenderInstanceMap.end()) {
		ThrowMyEx("meshName illegel!");
	}

	meshRenderInstanceMap[meshRenderCPT->mMeshName]->UpdateMeshRender(meshRenderCPT->mName, meshRenderCPT->GetWorld(), meshRenderCPT->mMaterial->mIndex, meshRenderCPT->mTexTransform, meshRenderCPT->mReceiveShadow);
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