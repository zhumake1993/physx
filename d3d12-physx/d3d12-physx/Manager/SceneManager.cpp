#include "SceneManager.h"

#include "Scenes/MainScene.h"
#include "Scenes/Linkup.h"

#include "Common/GameTimer.h"
extern GameTimer gTimer;

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

	gTimer.Reset();

	mNextScene = "";
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

void SceneManager::OnMouseDown(WPARAM btnState, int x, int y)
{
	mCurrScene->OnMouseDown(btnState, x, y);
}

void SceneManager::OnMouseUp(WPARAM btnState, int x, int y)
{
	mCurrScene->OnMouseUp(btnState, x, y);
}

void SceneManager::OnMouseMove(WPARAM btnState, int x, int y)
{
	mCurrScene->OnMouseMove(btnState, x, y);
}
