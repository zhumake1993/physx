#include "Scene.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

extern Setting gSetting;

#include "../physx/Main/PhysX.h"
extern PhysX gPhysX;

Scene::Scene()
{
	mGameObjectManager = std::make_shared<GameObjectManager>();
	mMeshRenderInstanceManager = std::make_shared<MeshRenderInstanceManager>();
	mTextureManager = std::make_shared<TextureManager>();
	mMaterialManager = std::make_shared<MaterialManager>();
	mMeshManager = std::make_shared<MeshManager>();
	mInputManager = std::make_shared<InputManager>();

	mMainCamera = std::make_shared<Camera>();
	mMainCamera->SetLens(0.25f * MathHelper::Pi, static_cast<float>(gSetting.ClientWidth) / gSetting.ClientHeight, 1.0f, 1000.0f);
	mMainCamera->SetTranslation(0.0f, 2.0f, -15.0f);
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

void Scene::Update(const GameTimer& gt)
{
	gPhysX.Update(gt.DeltaTime());

	// 注意，更新顺序很重要！

	// 游戏物体的更新放在最前面，因为其包含了游戏的主要逻辑，是游戏数据发生改变的原因
	mGameObjectManager->Update(gt);
}

void Scene::PostUpdate(const GameTimer& gt)
{
	// 由于游戏逻辑可能修改了材质，因此材质的更新要放在游戏逻辑的更新的后面
	mMaterialManager->UploadMaterial();

	// 由于游戏逻辑可能修改了渲染实例，因此渲染实例的更新要放在游戏逻辑的更新的后面
	mMeshRenderInstanceManager->UploadMeshRender();

	// 注意，输入管理器的更新函数要放在最后，否则GetKeyDown之类的函数会失效
	mInputManager->Update(gt);
}

void Scene::Pick(int sx, int sy)
{
	XMFLOAT4X4 P = mMainCamera->GetProj4x4f();

	// 计算视空间的选取射线
	float vx = (+2.0f * sx / gSetting.ClientWidth - 1.0f) / P(0, 0);
	float vy = (-2.0f * sy / gSetting.ClientHeight + 1.0f) / P(1, 1);

	// 视空间的射线定义
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	// 将射线转换至世界空间
	XMMATRIX V = mMainCamera->GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	XMVECTOR rayOriginW = XMVector3TransformCoord(rayOrigin, invView);
	XMVECTOR rayDirW = XMVector3TransformNormal(rayDir, invView);

	mMeshRenderInstanceManager->Pick(rayOriginW, rayDirW);
}