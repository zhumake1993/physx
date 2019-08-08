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
	// 清空PhysX的场景
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

//===========================================================
//===========================================================
// 方便函数
//===========================================================
//===========================================================

extern std::unique_ptr<SceneManager> gSceneManager;

void SwitchCurrScene(std::string name) { gSceneManager->SwitchScene(name); }

std::shared_ptr<Camera> GetCurrMainCamera() { return gSceneManager->GetCurrScene()->mMainCamera; }
void SetCurrMainCamera(std::shared_ptr<Camera> camera) { gSceneManager->GetCurrScene()->mMainCamera = camera; }

std::shared_ptr<GameObjectManager> GetCurrGameObjectManager() { return gSceneManager->GetCurrScene()->mGameObjectManager; }
std::shared_ptr<MeshRenderInstanceManager> GetCurrMeshRenderInstanceManager() { return gSceneManager->GetCurrScene()->mMeshRenderInstanceManager; }
std::shared_ptr<TextureManager> GetCurrTextureManager() { return gSceneManager->GetCurrScene()->mTextureManager; }
std::shared_ptr<MaterialManager> GetCurrMaterialManager() { return gSceneManager->GetCurrScene()->mMaterialManager; }
std::shared_ptr<MeshManager> GetCurrMeshManager() { return gSceneManager->GetCurrScene()->mMeshManager; }
std::shared_ptr<InputManager> GetCurrInputManager() { return gSceneManager->GetCurrScene()->mInputManager; }

bool& GetCurrIsWireframe() { return gSceneManager->GetCurrScene()->mIsWireframe; }
bool& GetCurrIsDepthComplexityUseStencil() { return gSceneManager->GetCurrScene()->mIsDepthComplexityUseStencil; }
bool& GetCurrIsDepthComplexityUseBlend() { return gSceneManager->GetCurrScene()->mIsDepthComplexityUseBlend; }
bool& GetCurrIsCubeMap() { return gSceneManager->GetCurrScene()->mIsCubeMap; }
bool& GetCurrIsShadowMap() { return gSceneManager->GetCurrScene()->mIsShadowMap; }
bool& GetCurrIsSsao() { return gSceneManager->GetCurrScene()->mIsSsao; }

bool& GetCurrIsBlur() { return gSceneManager->GetCurrScene()->mIsBlur; }
bool& GetCurrIsSobel() { return gSceneManager->GetCurrScene()->mIsSobel; }

bool& GetCurrIsDrawRigidbody() { return gSceneManager->GetCurrScene()->mIsDrawRigidbody; }
