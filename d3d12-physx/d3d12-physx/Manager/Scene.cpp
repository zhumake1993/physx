#include "Scene.h"

Scene::Scene()
{
	mGameObjectManager = std::make_shared<GameObjectManager>();
	mInstanceManager = std::make_shared<InstanceManager>();
	mTextureManager = std::make_shared<TextureManager>();
	mMaterialManager = std::make_shared<MaterialManager>();
	mMeshManager = std::make_shared<MeshManager>();
	mInputManager = std::make_shared<InputManager>();
}

Scene::~Scene()
{
}

void Scene::Initialize()
{
	BuildManagers();
	BuildTextures();
	BuildMaterials();
	BuildMeshes();
	BuildGameObjects();
}

std::shared_ptr<GameObjectManager> Scene::GetGameObjectManager()
{
	return mGameObjectManager;
}

std::shared_ptr<InstanceManager> Scene::GetInstanceManager()
{
	return mInstanceManager;
}

std::shared_ptr<TextureManager> Scene::GetTextureManager()
{
	return mTextureManager;
}

std::shared_ptr<MaterialManager> Scene::GetMaterialManager()
{
	return mMaterialManager;
}

std::shared_ptr<MeshManager> Scene::GetMeshManager()
{
	return mMeshManager;
}

std::shared_ptr<InputManager> Scene::GetInputManager()
{
	return mInputManager;
}

void Scene::Update()
{
	// 注意，更新顺序很重要！
	mMaterialManager->UpdateMaterialData();
	mGameObjectManager->Update();
	mInputManager->Update();
	mInstanceManager->UploadInstanceData();
}