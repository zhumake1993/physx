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

std::shared_ptr<GameObject> GameObjectManager::GetGameObject(std::string name)
{
	if (!HasGameObject(name)) {
		ThrowMyEx("GameObject does not exist!")
	}

	return mGameObjects[name];
}

std::string GameObjectManager::NewGameObjectName()
{
	auto name = MathHelper::RandStr();
	while (mGameObjects.find(name) != mGameObjects.end()) {
		name = MathHelper::RandStr();
	}

	return name;
}

void GameObjectManager::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
	mGameObjects[gameObject->mName] = gameObject;
}

void GameObjectManager::DeleteGameObject(std::string name)
{
	if (!HasGameObject(name)) {
		ThrowMyEx("GameObject does not exist!")
	}

	mToBeDeleted.insert(name);
}

void GameObjectManager::Update(const GameTimer& gt)
{
	for (auto it = mGameObjects.begin(); it != mGameObjects.end(); ++it) {
		it->second->Update(gt);
	}

	for (auto it = mGameObjects.begin(); it != mGameObjects.end();) {

		if (mToBeDeleted.find(it->first) != mToBeDeleted.end()) {
			it->second->Release();
			mToBeDeleted.erase(it->first);
			it = mGameObjects.erase(it);
		}
		else {
			++it;
		}
	}
}