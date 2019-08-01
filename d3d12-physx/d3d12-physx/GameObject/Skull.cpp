#include "Skull.h"

using namespace DirectX;

Skull::Skull(const Transform& transform)
	:GameObject(transform)
{
	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(transform);
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

void Skull::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	mTransform.Translation = XMFLOAT3(3.0f * cos(gt.TotalTime()*0.5f), 2.0f, 3.0f * sin(gt.TotalTime()*0.5f));
	XMStoreFloat4(&mTransform.Quaternion, XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), gt.TotalTime()));

	if (GetKeyDown('X')) {
		SwitchScene("Linkup");
	}
}
