#include "Cube.h"

using namespace DirectX;

#include "Manager/SceneManager.h"
extern std::unique_ptr<SceneManager> gSceneManager;

Cube::Cube()
	:GameObject()
{
	// 基础信息
	mName = "Cube";

	// MeshRender
	mHasMeshRender = true;
	mTransform.Translation = XMFLOAT3(0.0f, 10.0f, 0.0f);
	mTransform.Scale = XMFLOAT3(0.99f, 0.99f, 0.99f);
	mMeshRender.MatName = "bricks";
	XMStoreFloat4x4(&mMeshRender.TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
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

Cube::~Cube()
{
}

void Cube::Update()
{
	GameObject::Update();

	if (GetKeyDown('Z')) {
		gSceneManager->SwitchScene("MainScene");
	}
}
