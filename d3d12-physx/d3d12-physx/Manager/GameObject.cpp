#include "GameObject.h"
#include "Manager/SceneManager.h"

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

	if (mIsStatic) {
		return;
	}

	if (mRigidDynamicCPT) {
		mRigidDynamicCPT->Update(mTransform);
	}

	if (mMeshRenderCPT) {
		mMeshRenderCPT->Update(mTransform);
	}

	if (mCameraCPT) {
		mCameraCPT->Update(mTransform);
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

bool GameObject::GetKeyDown(int key) { return GetCurrInputManager()->GetKeyDown(key); }
bool GameObject::GetKeyPress(int key) { return GetCurrInputManager()->GetKeyPress(key); }
bool GameObject::GetKeyUp(int key) { return GetCurrInputManager()->GetKeyUp(key); }
bool GameObject::GetMouseDown(int key) { return GetCurrInputManager()->GetMouseDown(key); }
bool GameObject::GetMousePress(int key) { return GetCurrInputManager()->GetMousePress(key); }
bool GameObject::GetMouseUp(int key) { return GetCurrInputManager()->GetMouseUp(key); }
int GameObject::GetMouseX() { return GetCurrInputManager()->GetMouseX(); }
int GameObject::GetMouseY() { return GetCurrInputManager()->GetMouseY(); }

bool GameObject::HasGameObject(std::string name) { return GetCurrGameObjectManager()->HasGameObject(name); }
std::shared_ptr<GameObject> GameObject::GetGameObject(std::string name) { return GetCurrGameObjectManager()->GetGameObject(name); }
void GameObject::AddGameObject(std::shared_ptr<GameObject> gameObject) { GetCurrGameObjectManager()->AddGameObject(gameObject); }
void GameObject::DeleteGameObject(std::string name) { GetCurrGameObjectManager()->DeleteGameObject(name); }

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
	SwitchCurrScene(name);
}

std::shared_ptr<Camera> GameObject::GetMainCamera()
{
	return GetCurrMainCamera();
}

void GameObject::Destroy(float time)
{
	mGameTimer = std::make_unique<GameTimer>();
	mGameTimer->Reset();
	mLifeTime = time;
}

bool GameObject::GetIsWireframe() { return GetCurrIsWireframe(); }
bool GameObject::GetIsDepthComplexityUseStencil() { return GetCurrIsDepthComplexityUseStencil(); }
bool GameObject::GetIsDepthComplexityUseBlend() { return GetCurrIsDepthComplexityUseBlend(); }
bool GameObject::GetIsCubeMap() { return GetCurrIsCubeMap(); }
bool GameObject::GetIsShadowMap() { return GetCurrIsShadowMap(); }
bool GameObject::GetIsSsao() { return GetCurrIsSsao(); }
bool GameObject::GetIsBlur() { return GetCurrIsBlur(); }
bool GameObject::GetIsSobel() { return GetCurrIsSobel(); }
bool GameObject::GetIsDrawRigidbody() { return GetCurrIsDrawRigidbody(); }

void GameObject::SetIsWireframe(bool st) { GetCurrIsWireframe() = st; }
void GameObject::SetIsDepthComplexityUseStencil(bool st) { GetCurrIsDepthComplexityUseStencil() = st; }
void GameObject::SetIsDepthComplexityUseBlend(bool st) { GetCurrIsDepthComplexityUseBlend() = st; }
void GameObject::SetIsCubeMap(bool st) { GetCurrIsCubeMap() = st; }
void GameObject::SetIsShadowMap(bool st) { GetCurrIsShadowMap() = st; }
void GameObject::SetIsSsao(bool st) { GetCurrIsSsao() = st; }
void GameObject::SetIsBlur(bool st) { GetCurrIsBlur() = st; }
void GameObject::SetIsSobel(bool st) { GetCurrIsSobel() = st; }
void GameObject::SetIsDrawRigidbody(bool st) { GetCurrIsDrawRigidbody() = st; }
