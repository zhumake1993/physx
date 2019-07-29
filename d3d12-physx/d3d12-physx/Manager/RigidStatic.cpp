#include "RigidStatic.h"

using namespace DirectX;

#include "Manager/SceneManager.h"
extern std::unique_ptr<SceneManager> gSceneManager;

#include "../physx/Main/PhysX.h"
extern PhysX gPhysX;

extern XMMATRIX TransformToMatrix(Transform& transform);

RigidStatic::RigidStatic(const std::string& name, const Transform& parent, const Transform& local)
{
	mName = name;
	mParentTransform = parent;
	mLocalTransform = local;
}

RigidStatic::~RigidStatic()
{
}

void RigidStatic::AddRigidStatic()
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

	gPhysX.CreatePxRigidStatic(mName, &desc);

	// 添加刚体MeshRender

	GeometryGenerator geoGen;
	switch (mPxGeometry) {
		case PxSphereEnum: {
			assert(false);
			break;
		}
		case PxBoxEnum: {
			gSceneManager->GetCurrMeshManager()->AddMesh(mName + "RigidStaticMesh", geoGen.CreateBox(mScale.x * 2, mScale.y * 2, mScale.z * 2, 0));
			break;
		}
		case PxCapsuleEnum: {
			assert(false);
			break;
		}
		case PxPlaneEnum: {
			assert(false);
			break;
		}
		default: {
			assert(false);
		}
	}

	mMeshRender = std::make_unique<MeshRender>(mName + "RigidStatic", Transform(worldPos, worldQuat));
	mMeshRender->mMatName = "null";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = mName + "RigidStaticMesh";
	mMeshRender->mRenderLayer = (int)RenderLayer::Wireframe;
	mMeshRender->mReceiveShadow = false;
	mMeshRender->AddMeshRender();
}

void RigidStatic::Release()
{
	gPhysX.DeletePxRigid(mName);
	mMeshRender->Release();
}
