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

	// 计算刚体的世界坐标
	auto localMatrix = TransformToMatrix(mLocalTransform);
	auto worldMatrix = localMatrix * TransformToMatrix(mParentTransform);
	XMVECTOR worldPosV;
	XMVECTOR worldQuatV;
	XMVECTOR worldScaleV;
	XMMatrixDecompose(&worldScaleV, &worldQuatV, &worldPosV, worldMatrix);

	XMStoreFloat3(&mWorldTransform.Translation, worldPosV);
	XMStoreFloat4(&mWorldTransform.Quaternion, worldQuatV);

	// 主体相对于刚体的局部坐标
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

	desc.pos.x = mWorldTransform.Translation.x;
	desc.pos.y = mWorldTransform.Translation.y;
	desc.pos.z = mWorldTransform.Translation.z;

	desc.quat.x = mWorldTransform.Quaternion.x;
	desc.quat.y = mWorldTransform.Quaternion.y;
	desc.quat.z = mWorldTransform.Quaternion.z;
	desc.quat.w = mWorldTransform.Quaternion.w;

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
	gPhysX.SetKinematicFlag(mName, isKinematic);

	// 添加刚体MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(Transform(mWorldTransform.Translation, mWorldTransform.Quaternion, XMFLOAT3(mScale.x * 2, mScale.y * 2, mScale.z * 2)));
	mMeshRenderCPT->mMaterial = GetCurrMaterialManager()->GetDefaultMaterial();
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRenderCPT->mMeshName = "UnitBox";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Wireframe;
	mMeshRenderCPT->mReceiveShadow = false;
	mMeshRenderCPT->AddMeshRender();
}

void RigidDynamicCPT::Update(Transform& parentTransform)
{
	if (isKinematic) {

		PushTransform(parentTransform);
	}
	else {

		PullTransform();
		
		parentTransform.Translation = mParentTransform.Translation;
		parentTransform.Quaternion = mParentTransform.Quaternion;
	}

	// 更新刚体MeshRender的世界坐标
	mMeshRenderCPT->Update(mWorldTransform);
}

void RigidDynamicCPT::Release()
{
	gPhysX.DeletePxRigidDynamic(mName);
	mMeshRenderCPT->Release();
}

void RigidDynamicCPT::AddForce(DirectX::XMFLOAT3 force)
{
	gPhysX.AddForce(mName, PxFloat3(force.x, force.y, force.z));
}

void RigidDynamicCPT::SetRigidDynamicLockFlag(int axis, bool st)
{
	gPhysX.SetRigidDynamicLockFlag(mName, axis, st);
}

void RigidDynamicCPT::SetAngularDamping(float ad)
{
	gPhysX.SetAngularDamping(mName, ad);
}

void RigidDynamicCPT::SetLinearVelocity(DirectX::XMFLOAT3 v)
{
	gPhysX.SetLinearVelocity(mName, PxFloat3(v.x, v.y, v.z));
}

void RigidDynamicCPT::PushTransform(const Transform& parentTransform)
{
	mParentTransform = parentTransform;

	// 计算刚体的世界坐标
	auto localMatrix = TransformToMatrix(mLocalTransform);
	auto worldMatrix = localMatrix * TransformToMatrix(mParentTransform);
	XMVECTOR worldPosV;
	XMVECTOR worldQuatV;
	XMVECTOR worldScaleV;
	XMMatrixDecompose(&worldScaleV, &worldQuatV, &worldPosV, worldMatrix);

	XMStoreFloat3(&mWorldTransform.Translation, worldPosV);
	XMStoreFloat4(&mWorldTransform.Quaternion, worldQuatV);

	PxFloat3 pos = PxFloat3(mWorldTransform.Translation.x, mWorldTransform.Translation.y, mWorldTransform.Translation.z);
	PxFloat4 quat = PxFloat4(mWorldTransform.Quaternion.x, mWorldTransform.Quaternion.y, mWorldTransform.Quaternion.z, mWorldTransform.Quaternion.w);
	gPhysX.SetKinematicTarget(mName, pos, quat);
}

void RigidDynamicCPT::PullTransform()
{
	PxFloat3 pos;
	PxFloat4 quat;
	gPhysX.GetPxRigidDynamicTransform(mName, pos, quat);

	// 刚体的世界坐标
	mWorldTransform.Translation = XMFLOAT3(pos.x, pos.y, pos.z);
	mWorldTransform.Quaternion = XMFLOAT4(quat.x, quat.y, quat.z, quat.w);

	// 计算主体的世界坐标矩阵
	auto newWorld = TransformToMatrix(mLocalTransformReverse) * TransformToMatrix(mWorldTransform);

	// 将主体的世界坐标矩阵分解为Transform的形式
	XMVECTOR newWorldPosV;
	XMVECTOR newWorldQuatV;
	XMVECTOR newWorldScaleV;
	XMMatrixDecompose(&newWorldScaleV, &newWorldQuatV, &newWorldPosV, newWorld);

	// 更新主体的世界坐标
	XMStoreFloat3(&mParentTransform.Translation, newWorldPosV);
	XMStoreFloat4(&mParentTransform.Quaternion, newWorldQuatV);
}
