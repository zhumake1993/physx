#include "Scene.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

extern Setting gSetting;

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

	mMainCamera = std::make_shared<Camera>();
	mMainCamera->SetLens(0.25f * MathHelper::Pi, static_cast<float>(gSetting.ClientWidth) / gSetting.ClientHeight, 1.0f, 1000.0f);
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

std::shared_ptr<Camera> Scene::GetMainCamera()
{
	return mMainCamera;
}

void Scene::Update(const GameTimer& gt)
{
	gPhysX.Update(gt.DeltaTime());

	// ע�⣬����˳�����Ҫ��

	// ��Ϸ����ĸ��·�����ǰ�棬��Ϊ���������Ϸ����Ҫ�߼�������Ϸ���ݷ����ı��ԭ��
	mGameObjectManager->Update(gt);
}

void Scene::PostUpdate(const GameTimer& gt)
{
	// ������Ϸ�߼������޸��˲��ʣ���˲��ʵĸ���Ҫ������Ϸ�߼��ĸ��µĺ���
	mMaterialManager->UpdateMaterialData();

	// ������Ϸ�߼������޸�����Ⱦʵ���������Ⱦʵ���ĸ���Ҫ������Ϸ�߼��ĸ��µĺ���
	mInstanceManager->UploadInstanceData();

	// ע�⣬����������ĸ��º���Ҫ������󣬷���GetKeyDown֮��ĺ�����ʧЧ
	mInputManager->Update(gt);
}

void Scene::Pick(int sx, int sy)
{
	XMFLOAT4X4 P = mMainCamera->GetProj4x4f();

	// �����ӿռ��ѡȡ����
	float vx = (+2.0f * sx / gSetting.ClientWidth - 1.0f) / P(0, 0);
	float vy = (-2.0f * sy / gSetting.ClientHeight + 1.0f) / P(1, 1);

	// �ӿռ�����߶���
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	// ������ת��������ռ�
	XMMATRIX V = mMainCamera->GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	XMVECTOR rayOriginW = XMVector3TransformCoord(rayOrigin, invView);
	XMVECTOR rayDirW = XMVector3TransformNormal(rayDir, invView);

	mInstanceManager->Pick(rayOriginW, rayDirW);
}