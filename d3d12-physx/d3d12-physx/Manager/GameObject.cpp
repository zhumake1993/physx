#include "GameObject.h"
#include "Manager/CurrManagers.h"

using namespace DirectX;

GameObject::GameObject(const Transform& transform, const std::string& name)
{
	if (name != "") {
		mName = name;
	}
	else {
		mName = GetCurrGameObjectManager()->NewGameObjectName();
	}

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

	if (mRigidDynamicCPT) {
		mRigidDynamicCPT->Update();
		mTransform.Translation = mRigidDynamicCPT->mParentTransform.Translation;
		mTransform.Quaternion = mRigidDynamicCPT->mParentTransform.Quaternion;
	}

	if (mMeshRenderCPT) {
		mMeshRenderCPT->mTransform.Translation = mTransform.Translation;
		mMeshRenderCPT->mTransform.Quaternion = mTransform.Quaternion;
		mMeshRenderCPT->Update();
	}
}

void GameObject::GetPicked(float dst, DirectX::XMFLOAT3 hitPoint)
{
}

void GameObject::Release()
{
	if (mMaterial) {
		GetCurrMaterialManager()->DeleteMaterial(mMaterial->mName);
	}

	if (mMeshRenderCPT) {
		mMeshRenderCPT->Release();
	}

	if (mRigidDynamicCPT) {
		mRigidDynamicCPT->Release();
	}

	if (mRigidStaticCPT) {
		mRigidStaticCPT->Release();
	}
}

bool GameObject::GetKeyDown(int key)
{
	return GetCurrInputManager()->GetKeyDown(key);
}

bool GameObject::GetKeyPress(int key)
{
	return GetCurrInputManager()->GetKeyPress(key);
}

bool GameObject::GetKeyUp(int key)
{
	return GetCurrInputManager()->GetKeyUp(key);
}

bool GameObject::HasGameObject(std::string name)
{
	return GetCurrGameObjectManager()->HasGameObject(name);
}

std::shared_ptr<GameObject> GameObject::GetGameObject(std::string name)
{
	return GetCurrGameObjectManager()->GetGameObject(name);
}

void GameObject::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
	GetCurrGameObjectManager()->AddGameObject(gameObject);
}

void GameObject::DeleteGameObject(std::string name)
{
	GetCurrGameObjectManager()->DeleteGameObject(name);
}

void GameObject::AddMaterial()
{
	GetCurrMaterialManager()->AddMaterial(mMaterial);
}

std::shared_ptr<Material> GameObject::GetMaterial()
{
	if (mMaterial) {
		return mMaterial;
	}
	else {
		return mMeshRenderCPT->mMaterial;
	}
}

void GameObject::SetMaterial(std::shared_ptr<Material> material)
{
	GetCurrMaterialManager()->SetMaterial(material);
}

int GameObject::GetTextureIndex(std::string name)
{
	return GetCurrTextureManager()->GetIndex(name);
}

void GameObject::SwitchScene(std::string name)
{
	SwitchScene(name);
}

void GameObject::Destroy(float time)
{
	mGameTimer = std::make_unique<GameTimer>();
	mGameTimer->Reset();
	mLifeTime = time;
}