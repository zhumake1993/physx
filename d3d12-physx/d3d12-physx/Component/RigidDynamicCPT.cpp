#include "RigidDynamicCPT.h"
#include "Manager/SceneManager.h"

using namespace DirectX;

#include "../physx/Main/PhysX.h"
extern PhysX gPhysX;

extern XMMATRIX TransformToMatrix(Transform& transform);

RigidDynamicCPT::RigidDynamicCPT(const Transform& parent, const Transform& local)
{
	mParentTransform = parent;
	mLocalTransform = local;

	// ��������ڸ���ľֲ�����
	Transform mLocalTransformReverse;
	mLocalTransformReverse.Translation = XMFLOAT3(-mLocalTransform.Translation.x,
		-mLocalTransform.Translation.y,
		-mLocalTransform.Translation.z);
	mLocalTransformReverse.Quaternion = XMFLOAT4(-mLocalTransform.Quaternion.x,
		-mLocalTransform.Quaternion.y,
		-mLocalTransform.Quaternion.z,
		mLocalTransform.Quaternion.w);
	mLocalTransformReverse.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

RigidDynamicCPT::~RigidDynamicCPT()
{
}

void RigidDynamicCPT::AddRigidDynamic()
{
	PxRigidDynamicDesc desc;

	// ����������������
	auto local = TransformToMatrix(mLocalTransform);
	auto world = local * TransformToMatrix(mParentTransform);
	XMVECTOR worldPosV;
	XMVECTOR worldQuatV;
	XMVECTOR worldScaleV;
	XMMatrixDecompose(&worldScaleV, &worldQuatV, &worldPosV, world);

	XMFLOAT3 worldPos;
	XMFLOAT4 worldQuat;
	XMStoreFloat3(&worldPos, worldPosV);
	XMStoreFloat4(&worldQuat, worldQuatV);

	desc.pos.x = worldPos.x;
	desc.pos.y = worldPos.y;
	desc.pos.z = worldPos.z;

	desc.quat.x = worldQuat.x;
	desc.quat.y = worldQuat.y;
	desc.quat.z = worldQuat.z;
	desc.quat.w = worldQuat.w;

	desc.material.x = mPxMaterial.x;
	desc.material.y = mPxMaterial.y;
	desc.material.z = mPxMaterial.z;

	desc.pxGeometry = mPxGeometry;
	desc.scale.x = mScale.x;
	desc.scale.y = mScale.y;
	desc.scale.z = mScale.z;
	desc.scale.w = mScale.w;

	desc.density = mDensity;

	mName = gPhysX.CreatePxRigidDynamic(&desc);

	// ��Ӹ���MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(Transform(worldPos, worldQuat,XMFLOAT3(mScale.x * 2, mScale.y * 2, mScale.z * 2)));
	mMeshRenderCPT->mMaterial = GetCurrMaterialManager()->GetDefaultMaterial();
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRenderCPT->mMeshName = "UnitBox";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Wireframe;
	mMeshRenderCPT->mReceiveShadow = false;
	mMeshRenderCPT->AddMeshRender();
}

void RigidDynamicCPT::Update()
{
	PxFloat3 pos;
	PxFloat4 quat;
	gPhysX.GetPxRigidDynamicTransform(mName, pos, quat);

	// �������������
	Transform rigidBodyWorld;
	rigidBodyWorld.Translation = XMFLOAT3(pos.x, pos.y, pos.z);
	rigidBodyWorld.Quaternion = XMFLOAT4(quat.x, quat.y, quat.z, quat.w);
	rigidBodyWorld.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f); // �����

	// ��������������������
	auto newWorld = TransformToMatrix(mLocalTransformReverse) * TransformToMatrix(rigidBodyWorld);

	// ������������������ֽ�ΪTransform����ʽ
	XMVECTOR newWorldPosV;
	XMVECTOR newWorldQuatV;
	XMVECTOR newWorldScaleV;
	XMMatrixDecompose(&newWorldScaleV, &newWorldQuatV, &newWorldPosV, newWorld);

	// �����������������
	XMStoreFloat3(&mParentTransform.Translation, newWorldPosV);
	XMStoreFloat4(&mParentTransform.Quaternion, newWorldQuatV);

	// ���¸���MeshRender����������
	mMeshRenderCPT->mTransform.Translation = rigidBodyWorld.Translation;
	mMeshRenderCPT->mTransform.Quaternion = rigidBodyWorld.Quaternion;
	mMeshRenderCPT->Update();
}

void RigidDynamicCPT::Release()
{
	gPhysX.DeletePxRigidDynamic(mName);
	mMeshRenderCPT->Release();
}

void RigidDynamicCPT::SetAngularDamping(float ad)
{
	gPhysX.SetAngularDamping(mName, ad);
}

void RigidDynamicCPT::SetLinearVelocity(DirectX::XMFLOAT3 v)
{
	gPhysX.SetLinearVelocity(mName, PxFloat3(v.x, v.y, v.z));
}
