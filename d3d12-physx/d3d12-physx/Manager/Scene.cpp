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

	// ע�⣬����˳�����Ҫ��

	// ��Ϸ����ĸ��·�����ǰ�棬��Ϊ���������Ϸ����Ҫ�߼�������Ϸ���ݷ����ı��ԭ��
	mGameObjectManager->Update(gt);
}

void Scene::PostUpdate(const GameTimer& gt)
{
	// ������Ϸ�߼������޸��˲��ʣ���˲��ʵĸ���Ҫ������Ϸ�߼��ĸ��µĺ���
	mMaterialManager->UploadMaterial();

	// ������Ϸ�߼������޸�����Ⱦʵ���������Ⱦʵ���ĸ���Ҫ������Ϸ�߼��ĸ��µĺ���
	mMeshRenderInstanceManager->UploadMeshRender();

	// ע�⣬����������ĸ��º���Ҫ������󣬷���GetKeyDown֮��ĺ�����ʧЧ
	mInputManager->Update(gt);
}