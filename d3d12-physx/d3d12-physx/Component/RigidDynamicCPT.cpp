#include "RigidDynamicCPT.h"
#include "Manager/SceneManager.h"

using namespace DirectX;

#include "../physx/Main/PhysX.h"
extern PhysX gPhysX;

RigidDynamicCPT::RigidDynamicCPT(const std::string& parentName, const Transform& parent, const Transform& local)
{
	mParentName = parentName;
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
	XMStoreFloat4x4(&mLocalMatrixReverse, XMMatrixInverse(&XMMatrixDeterminant(localMatrix), localMatrix));
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

	// 添加Mesh
	auto rigidbodyName = mParentName + "Rigidbody";

	if (!GetCurrMeshManager()->HasMesh(rigidbodyName)) {

		GeometryGenerator geoGen;
		switch (mPxGeometry) {
			case PxBoxEnum:GetCurrMeshManager()->AddMesh(rigidbodyName, geoGen.CreateBox(mScale.x * 2, mScale.y * 2, mScale.z * 2, 0)); break;
			case PxSphereEnum:GetCurrMeshManager()->AddMesh(rigidbodyName, geoGen.CreateSphere(mScale.x, 20, 20)); break;
			case PxCapsuleEnum:GetCurrMeshManager()->AddMesh(rigidbodyName, geoGen.CreateCapsule(mScale.x, mScale.y * 2, 20, 10, 10)); break;
			default:ThrowMyEx("Wrong geometry!");
		}
	}

	// 添加刚体MeshRender

	Transform meshTransform = mWorldTransform;
	if (mPxGeometry == PxCapsuleEnum) {
		meshTransform = RotateTransformLocal(mWorldTransform, mWorldTransform.GetForward(), -XM_PIDIV2);
	}

	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(meshTransform);
	mMeshRenderCPT->mMaterial = GetCurrMaterialManager()->GetDefaultMaterial();
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRenderCPT->mMeshName = rigidbodyName;
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

	Transform meshTransform = mWorldTransform;
	if (mPxGeometry == PxCapsuleEnum) {
		meshTransform = RotateTransformLocal(mWorldTransform, mWorldTransform.GetForward(), -XM_PIDIV2);
	}

	mMeshRenderCPT->Update(meshTransform);
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
	auto newWorld = XMLoadFloat4x4(&mLocalMatrixReverse) * TransformToMatrix(mWorldTransform);

	// 将主体的世界坐标矩阵分解为Transform的形式
	XMVECTOR newWorldPosV;
	XMVECTOR newWorldQuatV;
	XMVECTOR newWorldScaleV;
	XMMatrixDecompose(&newWorldScaleV, &newWorldQuatV, &newWorldPosV, newWorld);

	// 更新主体的世界坐标
	XMStoreFloat3(&mParentTransform.Translation, newWorldPosV);
	XMStoreFloat4(&mParentTransform.Quaternion, newWorldQuatV);
}
