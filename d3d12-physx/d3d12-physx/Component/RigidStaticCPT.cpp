#include "RigidStaticCPT.h"
#include "Manager/SceneManager.h"

using namespace DirectX;

#include "../physx/Main/PhysX.h"
extern PhysX gPhysX;

RigidStaticCPT::RigidStaticCPT(const std::string& parentName, const Transform& parent, const Transform& local)
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
}

RigidStaticCPT::~RigidStaticCPT()
{
}

void RigidStaticCPT::AddRigidStatic()
{
	PxRigidStaticDesc desc;

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

	mName = gPhysX.CreatePxRigidStatic(&desc);

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

void RigidStaticCPT::Release()
{
	gPhysX.DeletePxRigidStatic(mName);
	mMeshRenderCPT->Release();
}
