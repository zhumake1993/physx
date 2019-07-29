#include "SceneManager.h"

#include "Scenes/MainScene.h"
#include "Scenes/Linkup.h"

#include "../physx/Main/PhysX.h"
extern PhysX gPhysX;

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::Initialize()
{
	mSceneCreateLambda = {
		{"MainScene",[this]() {this->mCurrScene = std::make_shared<MainScene>(); }},
		{"Linkup",[this]() {this->mCurrScene = std::make_shared<Linkup>(); }}
	};

	mSceneCreateLambda["Linkup"]();
	mCurrScene->Initialize();
}

void SceneManager::SwitchScene(std::string name)
{
	if (mSceneCreateLambda.find(name) == mSceneCreateLambda.end()) {
		ThrowMyEx("Scene does not exist!");
	}

	mNextScene = name;
}

bool SceneManager::ChangingScene()
{
	return mNextScene != "";
}

void SceneManager::ChangeScene()
{
	// Çå¿ÕPhysXµÄ³¡¾°
	gPhysX.CleanupScene();

	mSceneCreateLambda[mNextScene]();
	mCurrScene->Initialize();

	mNextScene = "";
}

void SceneManager::Update(const GameTimer& gt)
{
	mCurrScene->Update(gt);
	mCurrScene->PostUpdate(gt);
}

std::shared_ptr<Scene> SceneManager::GetCurrScene()
{
	return mCurrScene;
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

std::shared_ptr<Camera> SceneManager::GetCurrMainCamera()
{
	return mCurrScene->GetMainCamera();
}
