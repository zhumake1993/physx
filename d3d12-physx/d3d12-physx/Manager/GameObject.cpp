#include "GameObject.h"

using namespace DirectX;

#include "InputManager.h"
#include "InstanceManager.h"
extern std::unique_ptr<InstanceManager> gInstanceManager;
extern std::unique_ptr<InputManager> gInputManager;

#include "../physx/Main/PhysX.h"
extern PhysX gPhysX;

// ��һ��Transformת���ɾ�����ʽ
XMMATRIX GetMatrix(Transform& transform)
{
	XMVECTOR S = XMLoadFloat3(&transform.Scale);
	XMVECTOR P = XMLoadFloat3(&transform.Translation);
	XMVECTOR Q = XMLoadFloat4(&transform.Quaternion);

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	return XMMatrixAffineTransformation(S, zero, Q, P);
}

GameObject::GameObject()
{
	// Ĭ�ϻ�����Ϣ
	mName = "null";
	mTransform.Translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mTransform.Quaternion = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mTransform.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	// Ĭ��MeshRender
	mMeshRender.MatName = "null";
	XMStoreFloat4x4(&mMeshRender.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender.MeshName = "null";
	mMeshRender.RenderLayer = (int)RenderLayer::Opaque;
	mMeshRender.ReceiveShadow = true;

	// Ĭ�ϸ���
	mRigidBodyLocalTransform.Translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mRigidBodyLocalTransform.Quaternion = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mRigidBodyLocalTransform.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f); // �����洢������ĳߴ�
	mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mPxGeometry = PxSphere;
	mDensity = 10.0f;

	// Ĭ�ϸ����MeshRender
	mRigidBodyMeshRender.MatName = "null";
	XMStoreFloat4x4(&mRigidBodyMeshRender.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mRigidBodyMeshRender.MeshName = "null";
	mRigidBodyMeshRender.RenderLayer = (int)RenderLayer::Wireframe;
	mRigidBodyMeshRender.ReceiveShadow = false;
}

GameObject::~GameObject()
{
}

void GameObject::Update()
{
	if (mHasRigidBody) {
		float x, y, z, a, b, c, d;
		gPhysX.Get(x, y, z, a, b, c, d);

		// �������������
		Transform rigidBodyWorld;
		rigidBodyWorld.Translation = XMFLOAT3(x, y, z);
		rigidBodyWorld.Quaternion = XMFLOAT4(a, b, c, d);
		rigidBodyWorld.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f); // �����

		// ��������ڸ���ľֲ�����
		Transform rigidBodyLocalTransformReverse;
		rigidBodyLocalTransformReverse.Translation = XMFLOAT3(-mRigidBodyLocalTransform.Translation.x, 
															  -mRigidBodyLocalTransform.Translation.y, 
															  -mRigidBodyLocalTransform.Translation.z);
		rigidBodyLocalTransformReverse.Quaternion = XMFLOAT4(-mRigidBodyLocalTransform.Quaternion.x, 
															 -mRigidBodyLocalTransform.Quaternion.y, 
															 -mRigidBodyLocalTransform.Quaternion.z, 
															 mRigidBodyLocalTransform.Quaternion.w);
		rigidBodyLocalTransformReverse.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

		// ��������������������
		auto newWorld = GetMatrix(rigidBodyLocalTransformReverse) * GetMatrix(rigidBodyWorld);

		// ������������������ֽ�ΪTransform����ʽ
		XMVECTOR newWorldPosV;
		XMVECTOR newWorldQuatV;
		XMVECTOR newWorldScaleV;
		XMMatrixDecompose(&newWorldScaleV, &newWorldQuatV, &newWorldPosV, newWorld);

		// �����������������
		XMStoreFloat3(&mTransform.Translation, newWorldPosV);
		XMStoreFloat4(&mTransform.Quaternion, newWorldQuatV);

		// ���¸������������
		XMStoreFloat4x4(&mRigidBodyMeshRender.World, GetMatrix(rigidBodyWorld));
		gInstanceManager->UpdateInstance(mRigidBodyMeshRender);
	}

	if (mHasMeshRender) {
		// �����⣬�ݻ�
		XMStoreFloat4x4(&mMeshRender.World, GetMatrix(mTransform));
		gInstanceManager->UpdateInstance(mMeshRender);
	}
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

void GameObject::AddMeshRender()
{
	mMeshRender.Name = mName;
	XMStoreFloat4x4(&mMeshRender.World, GetMatrix(mTransform));
	gInstanceManager->AddInstance(mMeshRender);
}

void GameObject::AddRigidBody()
{
	PxRigidDynamicDesc desc;

	// ����������������
	auto local = GetMatrix(mRigidBodyLocalTransform);
	auto world = local * GetMatrix(mTransform);
	XMVECTOR worldPosV;
	XMVECTOR worldQuatV;
	XMVECTOR worldScaleV;
	XMMatrixDecompose(&worldScaleV, &worldQuatV, &worldPosV, world);

	XMFLOAT3 worldPos;
	XMStoreFloat3(&worldPos, worldPosV);
	desc.px = worldPos.x;
	desc.py = worldPos.y;
	desc.pz = worldPos.z;

	XMFLOAT4 worldQuat;
	XMStoreFloat4(&worldQuat, worldQuatV);
	desc.qx = worldQuat.x;
	desc.qy = worldQuat.y;
	desc.qz = worldQuat.z;
	desc.qw = worldQuat.w;

	desc.materialStaticFriction = mPxMaterial.x;
	desc.materialDynamicFriction = mPxMaterial.y;
	desc.materialRestitution = mPxMaterial.z;

	desc.pxGeometry = mPxGeometry;
	desc.gx = mRigidBodyLocalTransform.Scale.x;
	desc.gy = mRigidBodyLocalTransform.Scale.y;
	desc.gz = mRigidBodyLocalTransform.Scale.z;

	desc.density = mDensity;

	gPhysX.CreatePxRigidDynamic(mName, &desc);

	// ��Ӹ����MeshRender
	mRigidBodyMeshRender.Name = mName + "RigidBody";
	XMStoreFloat4x4(&mRigidBodyMeshRender.World, world);
	gInstanceManager->AddInstance(mRigidBodyMeshRender);
}
