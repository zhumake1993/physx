#include "GameObjectManager.h"

std::unique_ptr<GameObjectManager> gGameObjectManager = std::make_unique<GameObjectManager>();

GameObjectManager::GameObjectManager()
{
}

GameObjectManager::~GameObjectManager()
{
}

void GameObjectManager::Initialize()
{
}

void GameObjectManager::AddGameObject(std::unique_ptr<GameObject> gameObject)
{
	if (mGameObjects.find(gameObject->mGameObjectName) != mGameObjects.end()) {
		OutputMessageBox("GameObject already exists!");
		return;
	}

	gInstanceManager->AddInstance(gameObject->mGameObjectName, gameObject->GetWorld(),
		gameObject->mMatName, gameObject->mTexTransform, gameObject->mMeshName, gameObject->mRenderLayer,
		gameObject->mReceiveShadow);

	mGameObjects[gameObject->mGameObjectName] = std::move(gameObject);
}

std::unique_ptr<GameObject>& GameObjectManager::GetGameObject(std::string name)
{
	return mGameObjects[name];
}

void GameObjectManager::Update()
{
	for (auto &p : mGameObjects) {
		p.second->Update();

		gInstanceManager->UpdateInstance(p.second->mGameObjectName, p.second->GetWorld(),
			p.second->mMatName, p.second->mTexTransform, p.second->mMeshName, p.second->mRenderLayer, p.second->mReceiveShadow);
	}
}