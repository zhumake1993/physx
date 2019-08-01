#include "RigidDynamic.h"

using namespace DirectX;

#include "Manager/SceneManager.h"
extern std::unique_ptr<SceneManager> gSceneManager;

#include "../physx/Main/PhysX.h"
extern PhysX gPhysX;

extern XMMATRIX TransformToMatrix(Transform& transform);

RigidDynamic::RigidDynamic(const Transform& parent, const Transform& local)
{
	mParentTransform = parent;
	mLocalTransform = local;

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

RigidDynamic::~RigidDynamic()
{
}

void RigidDynamic::AddRigidDynamic()
{
	PxRigidDynamicDesc desc;

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

	desc.density = mDensity;

	mName = gPhysX.CreatePxRigidDynamic(&desc);

	// 添加刚体MeshRender

	GeometryGenerator geoGen;
	switch (mPxGeometry) {
		case PxSphereEnum: {
			assert(false);
			break;
		}
		case PxBoxEnum: {
			gSceneManager->GetCurrMeshManager()->AddMesh(mName + "RigidDynamicMesh", geoGen.CreateBox(mScale.x * 2, mScale.y * 2, mScale.z * 2, 0));
			break;
		}
		case PxCapsuleEnum: {
			assert(false);
			break;
		}
		default: {
			assert(false);
		}
	}

	mMeshRender = std::make_unique<MeshRender>(Transform(worldPos, worldQuat));
	mMeshRender->mMatName = "null";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = mName + "RigidDynamicMesh";
	mMeshRender->mRenderLayer = (int)RenderLayer::Wireframe;
	mMeshRender->mReceiveShadow = false;
	mMeshRender->AddMeshRender();
}

void RigidDynamic::Update()
{
	PxFloat3 pos;
	PxFloat4 quat;
	gPhysX.GetPxRigidDynamicTransform(mName, pos, quat);

	// 刚体的世界坐标
	Transform rigidBodyWorld;
	rigidBodyWorld.Translation = XMFLOAT3(pos.x, pos.y, pos.z);
	rigidBodyWorld.Quaternion = XMFLOAT4(quat.x, quat.y, quat.z, quat.w);
	rigidBodyWorld.Scale = XMFLOAT3(1.0f, 1.0f, 1.0f); // 此项不用

	// 计算主体的世界坐标矩阵
	auto newWorld = TransformToMatrix(mLocalTransformReverse) * TransformToMatrix(rigidBodyWorld);

	// 将主体的世界坐标矩阵分解为Transform的形式
	XMVECTOR newWorldPosV;
	XMVECTOR newWorldQuatV;
	XMVECTOR newWorldScaleV;
	XMMatrixDecompose(&newWorldScaleV, &newWorldQuatV, &newWorldPosV, newWorld);

	// 更新主体的世界坐标
	XMStoreFloat3(&mParentTransform.Translation, newWorldPosV);
	XMStoreFloat4(&mParentTransform.Quaternion, newWorldQuatV);

	// 更新刚体MeshRender的世界坐标
	mMeshRender->mTransform.Translation = rigidBodyWorld.Translation;
	mMeshRender->mTransform.Quaternion = rigidBodyWorld.Quaternion;
	mMeshRender->Update();
}

void RigidDynamic::Release()
{
	gPhysX.DeletePxRigid(mName);
	mMeshRender->Release();
}

void RigidDynamic::SetAngularDamping(float ad)
{
	gPhysX.SetAngularDamping(mName, ad);
}

void RigidDynamic::SetLinearVelocity(DirectX::XMFLOAT3 v)
{
	gPhysX.SetLinearVelocity(mName, PxFloat3(v.x, v.y, v.z));
}
