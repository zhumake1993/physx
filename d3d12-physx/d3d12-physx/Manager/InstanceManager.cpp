#include "InstanceManager.h"

using namespace DirectX;

#include "Manager/SceneManager.h"
extern std::unique_ptr<SceneManager> gSceneManager;

InstanceManager::InstanceManager()
{
}

InstanceManager::~InstanceManager()
{
}

void InstanceManager::Initialize()
{
}

void InstanceManager::AddInstance(MeshRender* meshRender)
{
	auto world = meshRender->GetWorld();
	auto matName = meshRender->mMatName;
	auto texTransform = meshRender->mTexTransform;
	auto meshName = meshRender->mMeshName;
	auto randerLayer = meshRender->mRenderLayer;
	auto receiveShadow = meshRender->mReceiveShadow;

	if (randerLayer < 0 || randerLayer >= (int)RenderLayer::Count) {
		ThrowMyEx("RenderLayer illegel!");
	}

	auto& instanceMap = mInstanceLayers[randerLayer];

	if (instanceMap.find(meshName) != instanceMap.end()) {
		meshRender->mName = instanceMap[meshName]->AddInstanceData(world, gSceneManager->GetCurrMaterialManager()->GetIndex(matName), texTransform, receiveShadow);
	}
	else {
		auto instance = std::make_unique<Instance>();
		instance->mMeshName = meshName;
		instance->mMesh = gSceneManager->GetCurrMeshManager()->GetMesh(meshName);
		instance->CalculateBoundingBox();

		meshRender->mName = instance->AddInstanceData(world, gSceneManager->GetCurrMaterialManager()->GetIndex(matName), texTransform, receiveShadow);

		instanceMap[meshName] = std::move(instance);
	}

	mMeshRenders[meshRender->mName] = meshRender;
}

void InstanceManager::DeleteInstance(MeshRender* meshRender)
{
	auto name = meshRender->mName;
	auto meshName = meshRender->mMeshName;
	auto randerLayer = meshRender->mRenderLayer;

	if (randerLayer < 0 || randerLayer >= (int)RenderLayer::Count) {
		ThrowMyEx("RenderLayer illegel!");
	}

	auto& instanceMap = mInstanceLayers[randerLayer];

	if (instanceMap.find(meshName) == instanceMap.end()) {
		ThrowMyEx("meshName illegel!");
	}

	instanceMap[meshName]->DeleteInstanceData(name);
}

void InstanceManager::UpdateInstance(MeshRender* meshRender)
{
	auto name = meshRender->mName;
	auto world = meshRender->GetWorld();
	auto matName = meshRender->mMatName;
	auto texTransform = meshRender->mTexTransform;
	auto meshName = meshRender->mMeshName;
	auto randerLayer = meshRender->mRenderLayer;
	auto receiveShadow = meshRender->mReceiveShadow;

	if (randerLayer < 0 || randerLayer >= (int)RenderLayer::Count) {
		ThrowMyEx("RenderLayer illegel!");
	}

	auto& instanceMap = mInstanceLayers[randerLayer];

	if (instanceMap.find(meshName) == instanceMap.end()) {
		ThrowMyEx("meshName illegel!");
	}

	instanceMap[meshName]->UpdateInstanceData(name, world, gSceneManager->GetCurrMaterialManager()->GetIndex(matName), texTransform, receiveShadow);
}

void InstanceManager::UploadInstanceData()
{
	for (auto &layer : mInstanceLayers) {
		for (auto& p : layer) {
			p.second->UploadInstanceData();
		}
	}
}

void InstanceManager::Draw(int randerLayer)
{
	for (auto& p : mInstanceLayers[randerLayer]) {
		p.second->Draw();
	}
}

bool InstanceManager::Pick(FXMVECTOR rayOriginW, FXMVECTOR rayDirW)
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

		for (auto& p : mInstanceLayers[i]) {

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

		gSceneManager->GetCurrGameObjectManager()->GetGameObject(mMeshRenders[nameResult]->GetParent())->GetPicked(tminResult, hitPoint);
	}

	return result;
}