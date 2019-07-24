#include "MainScene.h"

#include "GameObject/Sky.h"
#include "GameObject/Box.h"
#include "GameObject/Skull.h"
#include "GameObject/Globe.h"
#include "GameObject/Grid.h"
#include "GameObject/Cylinder.h"
#include "GameObject/Sphere.h"
#include "GameObject/BoxPx.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

#include "Common/GameTimer.h"
extern GameTimer gTimer;

#include "Common/Camera.h"
extern std::unique_ptr<Camera> gCamera;

MainScene::MainScene()
	:Scene()
{
}

MainScene::~MainScene()
{
}

void MainScene::Initialize()
{
	Scene::Initialize();

	gCamera->SetPosition(0.0f, 2.0f, -15.0f);
}

void MainScene::Update()
{
	Scene::Update();

	MoveCamera();
}

void MainScene::BuildManagers()
{
	mTextureManager->Initialize();
	mMaterialManager->Initialize();
	mMeshManager->Initialize();
	mInstanceManager->Initialize();
	mGameObjectManager->Initialize();
	mInputManager->Initialize();
}

void MainScene::BuildTextures()
{
	std::vector<std::wstring> fileNames =
	{
		L"Textures/bricks.dds",
		L"Textures/bricks_nmap.dds",
		L"Textures/bricks2.dds",
		L"Textures/bricks2_nmap.dds",
		L"Textures/stone.dds",
		L"Textures/tile.dds",
		L"Textures/tile_nmap.dds",
		L"Textures/white1x1.dds",
		L"Textures/default_nmap.dds",
		L"Textures/grass.dds",
		L"Textures/water1.dds",
		L"Textures/WoodCrate01.dds",
		L"Textures/WireFence.dds",
		L"Textures/ice.dds"
	};

	std::wstring cubeMapFileName = L"Textures/snowcube1024.dds";

	for (auto fileName : fileNames) {
		mTextureManager->AddTextureTex(fileName);
	}
	mTextureManager->AddTextureCube(cubeMapFileName);

	mTextureManager->BuildDescriptorHeaps();
}

void MainScene::BuildMaterials()
{
	auto bricks = std::make_shared<MaterialData>();
	bricks->DiffuseMapIndex = mTextureManager->GetIndex("bricks");
	bricks->NormalMapIndex = mTextureManager->GetIndex("bricks_nmap");
	bricks->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks->Roughness = 0.3f;
	mMaterialManager->AddMaterial("bricks", bricks);

	auto bricks2 = std::make_shared<MaterialData>();
	bricks2->DiffuseMapIndex = mTextureManager->GetIndex("bricks2");
	bricks2->NormalMapIndex = mTextureManager->GetIndex("bricks2_nmap");
	bricks2->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks2->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	bricks2->Roughness = 0.3f;
	mMaterialManager->AddMaterial("bricks2", bricks2);

	auto stone = std::make_shared<MaterialData>();
	stone->DiffuseMapIndex = mTextureManager->GetIndex("stone");
	stone->NormalMapIndex = -1;
	stone->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	stone->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	stone->Roughness = 0.1f;
	mMaterialManager->AddMaterial("stone", stone);

	auto tile = std::make_shared<MaterialData>();
	tile->DiffuseMapIndex = mTextureManager->GetIndex("tile");
	tile->NormalMapIndex = mTextureManager->GetIndex("tile_nmap");
	tile->DiffuseAlbedo = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	tile->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	tile->Roughness = 0.1f;
	mMaterialManager->AddMaterial("tile", tile);

	auto mirror0 = std::make_shared<MaterialData>();
	mirror0->DiffuseMapIndex = mTextureManager->GetIndex("white1x1");
	mirror0->NormalMapIndex = mTextureManager->GetIndex("default_nmap");
	mirror0->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mirror0->FresnelR0 = XMFLOAT3(0.98f, 0.97f, 0.95f);
	mirror0->Roughness = 0.1f;
	mMaterialManager->AddMaterial("mirror", mirror0);

	auto sky = std::make_shared<MaterialData>();
	sky->DiffuseMapIndex = mTextureManager->GetCubeIndex();
	sky->NormalMapIndex = -1;
	sky->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sky->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	sky->Roughness = 1.0f;
	mMaterialManager->AddMaterial("sky", sky);

	auto grass = std::make_shared<MaterialData>();
	grass->DiffuseMapIndex = mTextureManager->GetIndex("grass");
	grass->NormalMapIndex = -1;
	grass->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	grass->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	grass->Roughness = 0.925f;
	mMaterialManager->AddMaterial("grass", grass);

	auto water = std::make_shared<MaterialData>();
	water->DiffuseMapIndex = mTextureManager->GetIndex("water1");
	water->NormalMapIndex = -1;
	water->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	water->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	water->Roughness = 0.0f;
	mMaterialManager->AddMaterial("water", water);

	auto wirefence = std::make_shared<MaterialData>();
	wirefence->DiffuseMapIndex = mTextureManager->GetIndex("WireFence");
	wirefence->NormalMapIndex = -1;
	wirefence->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	wirefence->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	wirefence->Roughness = 0.25f;
	mMaterialManager->AddMaterial("wirefence", wirefence);

	auto ice = std::make_shared<MaterialData>();
	ice->DiffuseMapIndex = mTextureManager->GetIndex("ice");
	ice->NormalMapIndex = -1;
	ice->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	ice->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	ice->Roughness = 0.0f;
	mMaterialManager->AddMaterial("ice", ice);

	auto skullMat = std::make_shared<MaterialData>();
	skullMat->DiffuseMapIndex = mTextureManager->GetIndex("white1x1");
	skullMat->NormalMapIndex = -1;
	skullMat->DiffuseAlbedo = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	skullMat->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	skullMat->Roughness = 0.2f;
	mMaterialManager->AddMaterial("skullMat", skullMat);
}

void MainScene::BuildMeshes()
{
	GeometryGenerator geoGen;
	mMeshManager->AddMesh("box", geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3));
	mMeshManager->AddMesh("grid", geoGen.CreateGrid(20.0f, 30.0f, 60, 40));
	mMeshManager->AddMesh("sphere", geoGen.CreateSphere(0.5f, 20, 20));
	mMeshManager->AddMesh("cylinder", geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20));
	mMeshManager->AddMesh("box2", geoGen.CreateBox(8.0f, 8.0f, 8.0f, 3));
	mMeshManager->AddMesh("rigidBox", geoGen.CreateBox(1.0f, 1.0f, 1.0f, 0));
}

void MainScene::BuildGameObjects()
{
	auto sky = std::make_shared<Sky>();
	mGameObjectManager->AddGameObject(sky);

	auto box = std::make_shared<Box>();
	mGameObjectManager->AddGameObject(box);

	auto globe = std::make_shared<Globe>();
	mGameObjectManager->AddGameObject(globe);

	auto grid = std::make_shared<Grid>();
	mGameObjectManager->AddGameObject(grid);

	for (int i = 0; i < 5; ++i) {
		auto leftCyl = std::make_shared<Cylinder>();
		leftCyl->mName = "leftCyl" + std::to_string(i);
		leftCyl->mTransform.Translation = XMFLOAT3(-5.0f, 1.5f, -10.0f + i * 5.0f);
		mGameObjectManager->AddGameObject(leftCyl);

		auto rightCyl = std::make_shared<Cylinder>();
		rightCyl->mName = "rightCyl" + std::to_string(i);
		rightCyl->mTransform.Translation = XMFLOAT3(+5.0f, 1.5f, -10.0f + i * 5.0f);
		mGameObjectManager->AddGameObject(rightCyl);

		auto leftSphere = std::make_shared<Sphere>();
		leftSphere->mName = "leftSphere" + std::to_string(i);
		leftSphere->mTransform.Translation = XMFLOAT3(-5.0f, 3.5f, -10.0f + i * 5.0f);
		mGameObjectManager->AddGameObject(leftSphere);

		auto rightSphere = std::make_shared<Sphere>();
		rightSphere->mName = "rightSphere" + std::to_string(i);
		rightSphere->mTransform.Translation = XMFLOAT3(+5.0f, 3.5f, -10.0f + i * 5.0f);
		mGameObjectManager->AddGameObject(rightSphere);
	}

	auto skull = std::make_shared<Skull>();
	mGameObjectManager->AddGameObject(skull);

	auto boxPx = std::make_shared<BoxPx>();
	mGameObjectManager->AddGameObject(boxPx);
}

void MainScene::MoveCamera()
{
	const float dt = gTimer.DeltaTime();

	if (GetAsyncKeyState('W') & 0x8000)
		gCamera->Walk(10.0f * dt);

	if (GetAsyncKeyState('S') & 0x8000)
		gCamera->Walk(-10.0f * dt);

	if (GetAsyncKeyState('A') & 0x8000)
		gCamera->Strafe(-10.0f * dt);

	if (GetAsyncKeyState('D') & 0x8000)
		gCamera->Strafe(10.0f * dt);

	if (GetAsyncKeyState('Q') & 0x8000)
		gCamera->FlyUp(10.0f * dt);

	if (GetAsyncKeyState('E') & 0x8000)
		gCamera->FlyDown(10.0f * dt);
}

void MainScene::OnMouseDown(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) != 0) {
		mLastMousePos.x = x;
		mLastMousePos.y = y;
	}
	else if ((btnState & MK_LBUTTON) != 0) {
		Pick(x, y);
	}
}

void MainScene::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void MainScene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) != 0) {
		// 每像素对应0.25度
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		gCamera->Pitch(dy);
		gCamera->RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}