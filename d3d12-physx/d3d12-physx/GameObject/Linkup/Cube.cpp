#include "Cube.h"

using namespace DirectX;

#include "Manager/SceneManager.h"
extern std::unique_ptr<SceneManager> gSceneManager;

Cube::Cube(const std::string& name, const Transform& transform)
	:GameObject(name, transform)
{
	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(name, transform);
	mMeshRender->mMatName = "bricks";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = "box";
	mMeshRender->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRender->mReceiveShadow = true;
	mMeshRender->AddMeshRender();

	// ����
	Transform rigidDynamicLocal = Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	mRigidDynamic = std::make_unique<RigidDynamic>(name, transform, rigidDynamicLocal);
	mRigidDynamic->mScale = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	mRigidDynamic->mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mRigidDynamic->mPxGeometry = PxBoxEnum;
	mRigidDynamic->mDensity = 10.0f;
	mRigidDynamic->AddRigidDynamic();
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
