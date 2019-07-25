#include "Scene.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

extern int gClientWidth;
extern int gClientHeight;

#include "Common/GameTimer.h"
extern GameTimer gTimer;

#include "Common/Camera.h"
extern std::unique_ptr<Camera> gCamera;

#include "../physx/Main/PhysX.h"
extern PhysX gPhysX;

Scene::Scene()
{
	mGameObjectManager = std::make_shared<GameObjectManager>();
	mInstanceManager = std::make_shared<InstanceManager>();
	mTextureManager = std::make_shared<TextureManager>();
	mMaterialManager = std::make_shared<MaterialManager>();
	mMeshManager = std::make_shared<MeshManager>();
	mInputManager = std::make_shared<InputManager>();
}

Scene::~Scene()
{
}

void Scene::Initialize()
{
	gPhysX.CreateScene();

	BuildManagers();
	BuildTextures();
	BuildMaterials();
	BuildMeshes();
	BuildGameObjects();
}

std::shared_ptr<GameObjectManager> Scene::GetGameObjectManager()
{
	return mGameObjectManager;
}

std::shared_ptr<InstanceManager> Scene::GetInstanceManager()
{
	return mInstanceManager;
}

std::shared_ptr<TextureManager> Scene::GetTextureManager()
{
	return mTextureManager;
}

std::shared_ptr<MaterialManager> Scene::GetMaterialManager()
{
	return mMaterialManager;
}

std::shared_ptr<MeshManager> Scene::GetMeshManager()
{
	return mMeshManager;
}

std::shared_ptr<InputManager> Scene::GetInputManager()
{
	return mInputManager;
}

void Scene::Update()
{
	gPhysX.Update(gTimer.DeltaTime());

	// 注意，更新顺序很重要！
	mMaterialManager->UpdateMaterialData();
	mGameObjectManager->Update();
	mInputManager->Update();
	mInstanceManager->UploadInstanceData();
}

void Scene::MoveCamera()
{
}

void Scene::OnMouseDown(WPARAM btnState, int x, int y)
{
}

void Scene::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void Scene::OnMouseMove(WPARAM btnState, int x, int y)
{
}

void Scene::Pick(int sx, int sy)
{
	XMFLOAT4X4 P = gCamera->GetProj4x4f();

	// 计算视空间的选取射线
	float vx = (+2.0f * sx / gClientWidth - 1.0f) / P(0, 0);
	float vy = (-2.0f * sy / gClientHeight + 1.0f) / P(1, 1);

	// 视空间的射线定义
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	// 将射线转换至世界空间
	XMMATRIX V = gCamera->GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	XMVECTOR rayOriginW = XMVector3TransformCoord(rayOrigin, invView);
	XMVECTOR rayDirW = XMVector3TransformNormal(rayDir, invView);

	mInstanceManager->Pick(rayOriginW, rayDirW);
}