#include "BoxPx.h"

using namespace DirectX;

BoxPx::BoxPx()
	:GameObject()
{
	// 基础信息
	mName = "boxPx";

	// MeshRender
	mHasMeshRender = true;
	mTransform.Translation = XMFLOAT3(0.0f, 10.0f, -10.0f);
	mMeshRender.MatName = "bricks2";
	XMStoreFloat4x4(&mMeshRender.TexTransform, XMMatrixScaling(1.0f, 0.5f, 1.0f));
	mMeshRender.MeshName = "box";

	// 刚体
	mHasRigidBody = true;
	mRigidBodyLocalTransform.Translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mRigidBodyLocalTransform.Quaternion = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mRigidBodyLocalTransform.Scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mPxGeometry = PxBox;
	mDensity = 10.0f;
	mRigidBodyMeshRender.MeshName = "rigidBox";
}

BoxPx::~BoxPx()
{
}

void BoxPx::Update()
{
	GameObject::Update();
}
