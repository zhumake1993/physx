#include "GameObjectManager.h"

GameObjectManager::GameObjectManager()
{
}

GameObjectManager::~GameObjectManager()
{
}

void GameObjectManager::Initialize()
{
}

bool GameObjectManager::HasGameObject(std::string name)
{
	return mGameObjects.find(name) != mGameObjects.end();
}

bool GameObjectManager::HasGameObject(std::shared_ptr<GameObject> gameObject)
{
	return mGameObjects.find(gameObject->mName) != mGameObjects.end();
}

std::shared_ptr<GameObject> GameObjectManager::GetGameObject(std::string name)
{
	if (!HasGameObject(name)) {
		ThrowMyEx("GameObject does not exist!")
	}

	return mGameObjects[name];
}

void GameObjectManager::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
	int ran = rand();
	auto hash = std::hash<int>()(ran);
	while (mGameObjects.find(std::to_string(hash)) != mGameObjects.end()) {
		ran = rand();
		hash = std::hash<int>()(ran);
	}

	gameObject->mName = std::to_string(hash);

	if (gameObject->mMeshRender) {
		gameObject->mMeshRender->SetParent(gameObject->mName);
	}
	
	mGameObjects[gameObject->mName] = gameObject;
}

void GameObjectManager::AddGameObject(std::string name, std::shared_ptr<GameObject> gameObject)
{
	if (HasGameObject(name)) {
		ThrowMyEx("GameObject already exists!")
	}

	gameObject->mName = name;

	if (gameObject->mMeshRender) {
		gameObject->mMeshRender->SetParent(gameObject->mName);
	}

	mGameObjects[gameObject->mName] = gameObject;
}

void GameObjectManager::DeleteGameObject(std::string name)
{
	if (!HasGameObject(name)) {
		ThrowMyEx("GameObject does not exist!")
	}

	mGameObjects[name]->mToBeDeleted = true;
}

void GameObjectManager::DeleteGameObject(std::shared_ptr<GameObject> gameObject)
{
	if (!HasGameObject(gameObject->mName)) {
		ThrowMyEx("GameObject does not exist!")
	}

	gameObject->mToBeDeleted = true;
}

void GameObjectManager::Update(const GameTimer& gt)
{
	for (auto it = mGameObjects.begin(); it != mGameObjects.end();) {
		it->second->Update(gt);

		if (it->second->mToBeDeleted) {
			it->second->Release();
			it = mGameObjects.erase(it);
		}
		else {
			++it;
		}
	}
}