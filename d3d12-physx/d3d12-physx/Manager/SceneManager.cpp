#include "SceneManager.h"

#include "Scenes/MainScene.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Initialize()
{
	mCurrScene = std::make_shared<MainScene>();
	mCurrScene->Initialize();
}

void SceneManager::Update()
{
	mCurrScene->Update();
}

std::shared_ptr<GameObjectManager> SceneManager::GetCurrGameObjectManager()
{
	return mCurrScene->GetGameObjectManager();
}

std::shared_ptr<InstanceManager> SceneManager::GetCurrInstanceManager()
{
	return mCurrScene->GetInstanceManager();
}

std::shared_ptr<TextureManager> SceneManager::GetCurrTextureManager()
{
	return mCurrScene->GetTextureManager();
}

std::shared_ptr<MaterialManager> SceneManager::GetCurrMaterialManager()
{
	return mCurrScene->GetMaterialManager();
}

std::shared_ptr<MeshManager> SceneManager::GetCurrMeshManager()
{
	return mCurrScene->GetMeshManager();
}

std::shared_ptr<InputManager> SceneManager::GetCurrInputManager()
{
	return mCurrScene->GetInputManager();
}
