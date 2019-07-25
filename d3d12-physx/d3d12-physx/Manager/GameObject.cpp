#include "GameObject.h"

using namespace DirectX;

#include "Manager/SceneManager.h"
extern std::unique_ptr<SceneManager> gSceneManager;

GameObject::GameObject(const std::string& name, const Transform& transform)
{
	mName = name;
	mTransform = transform;
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
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
