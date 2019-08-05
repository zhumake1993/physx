#include "MainScene.h"

#include "GameObject/MainScene/MainSceneGameObjects.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

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

	mMainCamera->SetPosition(0.0f, 2.0f, -15.0f);
}

void MainScene::Update(const GameTimer& gt)
{
	Scene::Update(gt);

	MoveCamera(gt);

	if (mInputManager->GetMouseDown(0)) {
		Pick(mInputManager->GetMouseX(), mInputManager->GetMouseY());
	}

	if (mInputManager->GetMouseDown(1)) {
		mLastMousePos.x = mInputManager->GetMouseX();
		mLastMousePos.y = mInputManager->GetMouseY();
	}

	if (mInputManager->GetMousePress(1)) {
		// ÿ���ض�Ӧ0.25��
		float dx = XMConvertToRadians(0.25f * static_cast<float>(mInputManager->GetMouseX() - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(mInputManager->GetMouseY() - mLastMousePos.y));

		mMainCamera->Pitch(dy);
		mMainCamera->RotateY(dx);

		mLastMousePos.x = mInputManager->GetMouseX();
		mLastMousePos.y = mInputManager->GetMouseY();
	}
}

void MainScene::BuildManagers()
{
	mTextureManager->Initialize();
	mMaterialManager->Initialize();
	mMeshManager->Initialize();
	mMeshRenderInstanceManager->Initialize();
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
	//auto stone = std::make_shared<MaterialData>();
	//stone->DiffuseMapIndex = mTextureManager->GetIndex("stone");
	//stone->NormalMapIndex = -1;
	//stone->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//stone->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	//stone->Roughness = 0.1f;
	//mMaterialManager->AddMaterial("stone", stone);

	//auto grass = std::make_shared<MaterialData>();
	//grass->DiffuseMapIndex = mTextureManager->GetIndex("grass");
	//grass->NormalMapIndex = -1;
	//grass->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//grass->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
	//grass->Roughness = 0.925f;
	//mMaterialManager->AddMaterial("grass", grass);

	//auto water = std::make_shared<MaterialData>();
	//water->DiffuseMapIndex = mTextureManager->GetIndex("water1");
	//water->NormalMapIndex = -1;
	//water->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	//water->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	//water->Roughness = 0.0f;
	//mMaterialManager->AddMaterial("water", water);

	//auto wirefence = std::make_shared<MaterialData>();
	//wirefence->DiffuseMapIndex = mTextureManager->GetIndex("WireFence");
	//wirefence->NormalMapIndex = -1;
	//wirefence->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//wirefence->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	//wirefence->Roughness = 0.25f;
	//mMaterialManager->AddMaterial("wirefence", wirefence);

	//auto ice = std::make_shared<MaterialData>();
	//ice->DiffuseMapIndex = mTextureManager->GetIndex("ice");
	//ice->NormalMapIndex = -1;
	//ice->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//ice->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	//ice->Roughness = 0.0f;
	//mMaterialManager->AddMaterial("ice", ice);
}

void MainScene::BuildMeshes()
{
	GeometryGenerator geoGen;
	mMeshManager->AddMesh("box", geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3));
	mMeshManager->AddMesh("grid", geoGen.CreateGrid(20.0f, 30.0f, 60, 40));
	mMeshManager->AddMesh("sphere", geoGen.CreateSphere(0.5f, 20, 20));
	mMeshManager->AddMesh("cylinder", geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20));
	mMeshManager->AddMesh("box2", geoGen.CreateBox(8.0f, 8.0f, 8.0f, 3));

	mMeshManager->AddMesh("UnitBox", geoGen.CreateBox(1.0f, 1.0f, 1.0f, 0));
}

void MainScene::BuildGameObjects()
{
	auto sky = std::make_shared<Sky>(Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(5000.0f, 5000.0f, 5000.0f)), "Sky");
	mGameObjectManager->AddGameObject(sky);

	auto box = std::make_shared<Box>(Transform(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(2.0f, 1.0f, 2.0f)), "Box");
	mGameObjectManager->AddGameObject(box);

	auto globe = std::make_shared<Globe>(Transform(XMFLOAT3(0.0f, 2.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(2.0f, 2.0f, 2.0f)), "Globe");
	mGameObjectManager->AddGameObject(globe);

	auto grid = std::make_shared<Grid>(Transform(), "Grid");
	mGameObjectManager->AddGameObject(grid);

	for (int i = 0; i < 5; ++i) {
		auto leftCyl = std::make_shared<Cylinder>(Transform(XMFLOAT3(-5.0f, 1.5f, -10.0f + i * 5.0f)));
		mGameObjectManager->AddGameObject(leftCyl);

		auto rightCyl = std::make_shared<Cylinder>(Transform(XMFLOAT3(+5.0f, 1.5f, -10.0f + i * 5.0f)));
		mGameObjectManager->AddGameObject(rightCyl);

		auto leftSphere = std::make_shared<Sphere>(Transform(XMFLOAT3(-5.0f, 3.5f, -10.0f + i * 5.0f)));
		mGameObjectManager->AddGameObject(leftSphere);

		auto rightSphere = std::make_shared<Sphere>(Transform(XMFLOAT3(+5.0f, 3.5f, -10.0f + i * 5.0f)));
		mGameObjectManager->AddGameObject(rightSphere);
	}

	auto skull = std::make_shared<Skull>(Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(0.2f, 0.2f, 0.2f)), "Skull");
	mGameObjectManager->AddGameObject(skull);

	auto boxPx = std::make_shared<BoxPx>(Transform(XMFLOAT3(0.0f, 10.0f, -10.0f)), "BoxPx");
	mGameObjectManager->AddGameObject(boxPx);
}

void MainScene::MoveCamera(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (GetAsyncKeyState('W') & 0x8000)
		mMainCamera->Walk(10.0f * dt);

	if (GetAsyncKeyState('S') & 0x8000)
		mMainCamera->Walk(-10.0f * dt);

	if (GetAsyncKeyState('A') & 0x8000)
		mMainCamera->Strafe(-10.0f * dt);

	if (GetAsyncKeyState('D') & 0x8000)
		mMainCamera->Strafe(10.0f * dt);

	if (GetAsyncKeyState('Q') & 0x8000)
		mMainCamera->FlyUp(10.0f * dt);

	if (GetAsyncKeyState('E') & 0x8000)
		mMainCamera->FlyDown(10.0f * dt);
}