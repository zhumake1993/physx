#include "GameObject.h"

using namespace DirectX;

#include "Manager/SceneManager.h"
extern std::unique_ptr<SceneManager> gSceneManager;

GameObject::GameObject(const Transform& transform)
{
	mName = "";
	mTransform = transform;
}

GameObject::~GameObject()
{
}

void GameObject::Update(const GameTimer& gt)
{
	if (mGameTimer) {
		mGameTimer->Tick();
		if (mGameTimer->TotalTime() >= mLifeTime) {
			DeleteGameObject(mName);
			return;
		}
	}

	if (mRigidDynamic) {
		mRigidDynamic->Update();
		mTransform.Translation = mRigidDynamic->mParentTransform.Translation;
		mTransform.Quaternion = mRigidDynamic->mParentTransform.Quaternion;
	}

	if (mMeshRender) {
		mMeshRender->mTransform.Translation = mTransform.Translation;
		mMeshRender->mTransform.Quaternion = mTransform.Quaternion;
		mMeshRender->Update();
	}
}

void GameObject::GetPicked(float dst, DirectX::XMFLOAT3 hitPoint)
{
}

void GameObject::Release()
{
	if (mMeshRender) {
		mMeshRender->Release();
	}

	if (mRigidDynamic) {
		mRigidDynamic->Release();
	}

	if (mRigidStatic) {
		mRigidStatic->Release();
	}
}

bool GameObject::GetKeyDown(int key)
{
	return gSceneManager->GetCurrInputManager()->GetKeyDown(key);
}

bool GameObject::GetKeyPress(int key)
{
	return gSceneManager->GetCurrInputManager()->GetKeyPress(key);
}

bool GameObject::GetKeyUp(int key)
{
	return gSceneManager->GetCurrInputManager()->GetKeyUp(key);
}

bool GameObject::HasGameObject(std::string name)
{
	return gSceneManager->GetCurrGameObjectManager()->HasGameObject(name);
}

std::shared_ptr<GameObject> GameObject::GetGameObject(std::string name)
{
	return gSceneManager->GetCurrGameObjectManager()->GetGameObject(name);
}

void GameObject::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
	gSceneManager->GetCurrGameObjectManager()->AddGameObject(gameObject);
}

void GameObject::AddGameObject(std::string name, std::shared_ptr<GameObject> gameObject)
{
	gSceneManager->GetCurrGameObjectManager()->AddGameObject(name, gameObject);
}

void GameObject::DeleteGameObject(std::string name)
{
	gSceneManager->GetCurrGameObjectManager()->DeleteGameObject(name);
}

std::shared_ptr<MaterialData> GameObject::GetMaterial()
{
	if (!mMeshRender) {
		ThrowMyEx("The current gameobject does not have MeshRender!")
	}

	return gSceneManager->GetCurrMaterialManager()->GetMaterial(mMeshRender->mMatName);
}

void GameObject::AddMaterial(const std::string& name, std::shared_ptr<MaterialData> materialData)
{
	gSceneManager->GetCurrMaterialManager()->AddMaterial(name, materialData);
}

std::string GameObject::AddMaterial(std::shared_ptr<MaterialData> materialData)
{
	return gSceneManager->GetCurrMaterialManager()->AddMaterial(materialData);
}

int GameObject::GetTextureIndex(std::string name)
{
	return gSceneManager->GetCurrTextureManager()->GetIndex(name);
}

void GameObject::SwitchScene(std::string name)
{
	gSceneManager->SwitchScene(name);
}

void GameObject::Destroy(float time)
{
	mGameTimer = std::make_unique<GameTimer>();
	mGameTimer->Reset();
	mLifeTime = time;
}