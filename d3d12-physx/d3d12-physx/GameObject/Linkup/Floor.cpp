#include "Floor.h"

using namespace DirectX;

Floor::Floor(const std::string& name, const Transform& transform)
	:GameObject(name, transform)
{
	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(name, transform);
	mMeshRender->mMatName = "tile";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
	mMeshRender->mMeshName = "grid";
	mMeshRender->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRender->mReceiveShadow = true;
	mMeshRender->AddMeshRender();

	// ∏’ÃÂ
	Transform rigidStaticLocal = Transform(XMFLOAT3(0.0f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	mRigidStatic = std::make_unique<RigidStatic>(name, transform, rigidStaticLocal);
	mRigidStatic->mScale = XMFLOAT4(10.0f, 0.5f, 15.0f, 0.5f);
	mRigidStatic->mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.6f);
	mRigidStatic->mPxGeometry = PxBoxEnum;
	mRigidStatic->AddRigidStatic();
}

Floor::~Floor()
{
}

void Floor::Update()
{
	GameObject::Update();
}
