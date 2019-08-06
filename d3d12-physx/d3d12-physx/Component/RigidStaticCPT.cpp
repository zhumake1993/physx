#include "RigidStaticCPT.h"
#include "Manager/SceneManager.h"

using namespace DirectX;

#include "../physx/Main/PhysX.h"
extern PhysX gPhysX;

extern XMMATRIX TransformToMatrix(Transform& transform);

RigidStaticCPT::RigidStaticCPT(const Transform& parent, const Transform& local)
{
	mParentTransform = parent;
	mLocalTransform = local;
}

RigidStaticCPT::~RigidStaticCPT()
{
}

void RigidStaticCPT::AddRigidStatic()
{
	PxRigidStaticDesc desc;

	// 计算刚体的世界坐标
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

	mName = gPhysX.CreatePxRigidStatic(&desc);

	// 添加刚体MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(Transform(worldPos, worldQuat, XMFLOAT3(mScale.x * 2, mScale.y * 2, mScale.z * 2)));
	mMeshRenderCPT->mMaterial = GetCurrMaterialManager()->GetDefaultMaterial();
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRenderCPT->mMeshName = "UnitBox";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Wireframe;
	mMeshRenderCPT->mReceiveShadow = false;
	mMeshRenderCPT->AddMeshRender();
}

void RigidStaticCPT::Release()
{
	gPhysX.DeletePxRigidStatic(mName);
	mMeshRenderCPT->Release();
}
