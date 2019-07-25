#include "Skull.h"

using namespace DirectX;

#include "Common/GameTimer.h"
extern GameTimer gTimer;

#include "Manager/SceneManager.h"
extern std::unique_ptr<SceneManager> gSceneManager;

Skull::Skull(const std::string& name, const Transform& transform)
	:GameObject(name, transform)
{
	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(name, transform);
	mMeshRender->mMatName = "skullMat";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = "skull";
	mMeshRender->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRender->mReceiveShadow = true;
	mMeshRender->AddMeshRender();
}

Skull::~Skull()
{
}

void Skull::Update()
{
	GameObject::Update();

	mTransform.Translation = XMFLOAT3(3.0f * cos(gTimer.TotalTime()*0.5f), 2.0f, 3.0f * sin(gTimer.TotalTime()*0.5f));
	XMStoreFloat4(&mTransform.Quaternion, XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), gTimer.TotalTime()));

	if (GetKeyDown('X')) {
		gSceneManager->SwitchScene("Linkup");
	}
}
