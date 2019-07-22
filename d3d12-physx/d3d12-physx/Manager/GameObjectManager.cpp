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
	if (HasGameObject(gameObject)) {
		ThrowMyEx("GameObject already exists!")
	}

	if (gameObject->mHasMeshRender) {
		gameObject->AddMeshRender();
	}

	if (gameObject->mHasRigidBody) {
		gameObject->AddRigidBody();
	}

	mGameObjects[gameObject->mName] = gameObject;
}

void GameObjectManager::DeleteGameObject(std::string name)
{
	if (!HasGameObject(name)) {
		ThrowMyEx("GameObject does not exist!")
	}

	mGameObjects.erase(name);
}

void GameObjectManager::DeleteGameObject(std::shared_ptr<GameObject> gameObject)
{
	if (!HasGameObject(gameObject->mName)) {
		ThrowMyEx("GameObject does not exist!")
	}

	mGameObjects.erase(gameObject->mName);
}

void GameObjectManager::Update()
{
	for (auto p : mGameObjects) {
		p.second->Update();
	}
}