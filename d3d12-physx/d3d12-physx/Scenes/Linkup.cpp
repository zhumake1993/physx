#include "Linkup.h"

#include "GameObject/Linkup/LinkupGameObjects.h"
#include "GameObject/Sky.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

Linkup::Linkup()
	:Scene()
{
}

Linkup::~Linkup()
{
}

void Linkup::Initialize()
{
	Scene::Initialize();

	mMainCamera->SetPosition(0.0f, 2.0f, -15.0f);
}

void Linkup::Update(const GameTimer& gt)
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
		// 每像素对应0.25度
		float dx = XMConvertToRadians(0.25f * static_cast<float>(mInputManager->GetMouseX() - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(mInputManager->GetMouseY() - mLastMousePos.y));

		mMainCamera->Pitch(dy);
		mMainCamera->RotateY(dx);

		mLastMousePos.x = mInputManager->GetMouseX();
		mLastMousePos.y = mInputManager->GetMouseY();
	}
}

void Linkup::BuildManagers()
{
	mTextureManager->Initialize();
	mMaterialManager->Initialize();
	mMeshManager->Initialize();
	mInstanceManager->Initialize();
	mGameObjectManager->Initialize();
	mInputManager->Initialize();
}

void Linkup::BuildTextures()
{
	std::vector<std::wstring> fileNames =
	{
		L"Textures/bricks.dds",
		L"Textures/bricks_nmap.dds",
		L"Textures/tile.dds",
		L"Textures/tile_nmap.dds"
	};

	std::wstring cubeMapFileName = L"Textures/desertcube1024.dds";

	for (auto fileName : fileNames) {
		mTextureManager->AddTextureTex(fileName);
	}
	mTextureManager->AddTextureCube(cubeMapFileName);

	mTextureManager->BuildDescriptorHeaps();
}

void Linkup::BuildMaterials()
{
	auto sky = std::make_shared<MaterialData>();
	sky->DiffuseMapIndex = mTextureManager->GetCubeIndex();
	sky->NormalMapIndex = -1;
	sky->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sky->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	sky->Roughness = 1.0f;
	mMaterialManager->AddMaterial("sky", sky);

	auto bricks = std::make_shared<MaterialData>();
	bricks->DiffuseMapIndex = mTextureManager->GetIndex("bricks");
	bricks->NormalMapIndex = mTextureManager->GetIndex("bricks_nmap");
	bricks->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks->Roughness = 0.3f;
	mMaterialManager->AddMaterial("bricks", bricks);

	auto tile = std::make_shared<MaterialData>();
	tile->DiffuseMapIndex = mTextureManager->GetIndex("tile");
	tile->NormalMapIndex = mTextureManager->GetIndex("tile_nmap");
	tile->DiffuseAlbedo = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	tile->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	tile->Roughness = 0.1f;
	mMaterialManager->AddMaterial("tile", tile);

	auto segment = std::make_shared<MaterialData>();
	segment->DiffuseMapIndex = -1;
	segment->NormalMapIndex = -1;
	segment->DiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	segment->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	segment->Roughness = 0.1f;
	mMaterialManager->AddMaterial("Line", segment);
}

void Linkup::BuildMeshes()
{
	GeometryGenerator geoGen;
	mMeshManager->AddMesh("box", geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3));
	mMeshManager->AddMesh("grid", geoGen.CreateGrid(20.0f, 30.0f, 60, 40));
	mMeshManager->AddMesh("sphere", geoGen.CreateSphere(0.5f, 20, 20));

	mMeshManager->AddMesh("rigidBox", geoGen.CreateBox(1.0f, 1.0f, 1.0f, 0));

	mMeshManager->AddMesh("Segment", geoGen.CreateCylinder(1.0f, 1.0f, 1.0f, 20, 20));
	mMeshManager->AddMesh("Inflection", geoGen.CreateSphere(1.0f, 20, 20));
}

void Linkup::BuildGameObjects()
{
	auto logic = std::make_shared<Logic>("Logic");
	mGameObjectManager->AddGameObject(logic);

	auto sky = std::make_shared<Sky>("Sky", Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(5000.0f, 5000.0f, 5000.0f)));
	mGameObjectManager->AddGameObject(sky);

	auto cube = std::make_shared<Cube>("Cube", Transform(XMFLOAT3(0.0f, 5.0f, -5.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(0.99f, 0.99f, 0.99f)));
	mGameObjectManager->AddGameObject(cube);

	auto cubeGreen = std::make_shared<CubeGreen>("CubeGreen", Transform(XMFLOAT3(3.0f, 5.0f, -5.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(0.99f, 0.99f, 0.99f)));
	mGameObjectManager->AddGameObject(cubeGreen);

	auto floor = std::make_shared<Floor>("Floor");
	mGameObjectManager->AddGameObject(floor);
}

void Linkup::MoveCamera(const GameTimer& gt)
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