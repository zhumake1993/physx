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

void InstanceManager::AddInstance(const MeshRender& meshRender)
{
	auto name = meshRender.Name;
	auto world = meshRender.World;
	auto matName = meshRender.MatName;
	auto texTransform = meshRender.TexTransform;
	auto meshName = meshRender.MeshName;
	auto randerLayer = meshRender.RenderLayer;
	auto receiveShadow = meshRender.ReceiveShadow;

	auto& instanceMap = mInstanceLayers[randerLayer];

	if (instanceMap.find(meshName) != instanceMap.end()) {
		instanceMap[meshName]->AddInstanceData(name, world, gSceneManager->GetCurrMaterialManager()->GetIndex(matName), texTransform, receiveShadow);
	}
	else {
		auto instance = std::make_unique<Instance>();
		instance->mMeshName = meshName;
		instance->mMesh = gSceneManager->GetCurrMeshManager()->GetMesh(meshName);
		instance->CalculateBoundingBox();

		instance->AddInstanceData(name, world, gSceneManager->GetCurrMaterialManager()->GetIndex(matName), texTransform, receiveShadow);

		instanceMap[meshName] = std::move(instance);
	}
}

void InstanceManager::UpdateInstance(const MeshRender& meshRender)
{
	auto name = meshRender.Name;
	auto world = meshRender.World;
	auto matName = meshRender.MatName;
	auto texTransform = meshRender.TexTransform;
	auto meshName = meshRender.MeshName;
	auto randerLayer = meshRender.RenderLayer;
	auto receiveShadow = meshRender.ReceiveShadow;

	auto& instanceMap = mInstanceLayers[randerLayer];
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
		XMFLOAT3 pp;
		XMStoreFloat3(&pp, pointResult);
		OutputDebug(nameResult);
		OutputDebug(std::to_string(tminResult));
		OutputDebug(std::to_string(pp.x) + ',' + std::to_string(pp.y) + ',' + std::to_string(pp.z));
	}

	return result;
}