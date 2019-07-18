#include "GameObject.h"

void LocalToWorldTransform(XMFLOAT3& parentWorldPos, XMFLOAT4& parentWorldQuat, 
	XMFLOAT3& childLocalPos, XMFLOAT4& childLocalQuat,
	XMFLOAT3& childWorldPos, XMFLOAT4& childWorldQuat)
{
	XMVECTOR parentWorldPosV = XMLoadFloat3(&parentWorldPos);
	XMVECTOR parentWorldQuatV = XMLoadFloat4(&parentWorldQuat);
	XMVECTOR childLocalPosV = XMLoadFloat3(&childLocalPos);
	XMVECTOR childLocalQuatV = XMLoadFloat4(&childLocalQuat);

	XMVECTOR childWorldPosV = XMVector3Rotate(childLocalPosV, parentWorldQuatV) + parentWorldPosV;
	XMVECTOR childWorldQuatV = XMQuaternionMultiply(parentWorldQuatV, childLocalQuatV);

	XMStoreFloat3(&childWorldPos, childWorldPosV);
	XMStoreFloat4(&childWorldQuat, childWorldQuatV);
}

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

bool GameObject::GetKeyDown(int key)
{
	return gInputManager->GetKeyDown(key);
}

bool GameObject::GetKeyPress(int key)
{
	return gInputManager->GetKeyPress(key);
}

bool GameObject::GetKeyUp(int key)
{
	return gInputManager->GetKeyUp(key);
}

void GameObject::AddRigidBody()
{
	PxRigidDynamicDesc desc;

	XMFLOAT3 mPxWorldPos;
	XMFLOAT4 mPxWorldQuat;
	LocalToWorldTransform(mTranslation, mRotationQuat, mPxLocalPos, mPxLocalQuat, mPxWorldPos, mPxWorldQuat);

	desc.px = mPxWorldPos.x;
	desc.py = mPxWorldPos.y;
	desc.pz = mPxWorldPos.z;

	desc.qx = mPxWorldQuat.x;
	desc.qy = mPxWorldQuat.y;
	desc.qz = mPxWorldQuat.z;
	desc.qw = mPxWorldQuat.w;

	desc.materialStaticFriction = mPxMaterial.x;
	desc.materialDynamicFriction = mPxMaterial.y;
	desc.materialRestitution = mPxMaterial.z;

	desc.pxGeometry = mPxGeometry;
	desc.pxGeometryDesc = mPxGeometryDesc;

	desc.density = mDensity;

	gPhysX.CreatePxRigidDynamic(mGameObjectName, &desc);

	mHasRigidBody = true;
}

void GameObject::Update()
{
	if (mHasRigidBody) {
		float x, y, z, a, b, c, d;
		gPhysX.Get(x, y, z, a, b, c, d);

		XMFLOAT3 pxWorldPos = XMFLOAT3(x, y, z);
		XMFLOAT4 pxWorldQuat = XMFLOAT4(a, b, c, d);

		XMFLOAT3 revLocalPos = XMFLOAT3(-mPxLocalPos.x, -mPxLocalPos.y, -mPxLocalPos.z);
		XMFLOAT4 revLocalQuat = XMFLOAT4(-mPxLocalQuat.x, -mPxLocalQuat.y, -mPxLocalQuat.z, mPxLocalQuat.w);

		LocalToWorldTransform(pxWorldPos, pxWorldQuat, revLocalPos, revLocalQuat, mTranslation, mRotationQuat);
	}
}

XMFLOAT4X4 GameObject::GetWorld()
{
	XMFLOAT4X4 world;

	XMVECTOR S = XMLoadFloat3(&mScale);
	XMVECTOR P = XMLoadFloat3(&mTranslation);
	XMVECTOR Q = XMLoadFloat4(&mRotationQuat);

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&world, XMMatrixAffineTransformation(S, zero, Q, P));

	return world;
}

