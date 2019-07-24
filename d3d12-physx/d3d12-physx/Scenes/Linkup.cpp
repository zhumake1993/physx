#include "Linkup.h"

#include "GameObject/Sky.h"
#include "GameObject/Linkup/Cube.h"
#include "GameObject/Linkup/Floor.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

#include "Common/GameTimer.h"
extern GameTimer gTimer;

#include "Common/Camera.h"
extern std::unique_ptr<Camera> gCamera;

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

	gCamera->SetPosition(0.0f, 2.0f, -15.0f);
}

void Linkup::Update()
{
	Scene::Update();

	MoveCamera();
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
}

void Linkup::BuildMeshes()
{
	GeometryGenerator geoGen;
	mMeshManager->AddMesh("box", geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3));
	mMeshManager->AddMesh("grid", geoGen.CreateGrid(20.0f, 30.0f, 60, 40));
	mMeshManager->AddMesh("sphere", geoGen.CreateSphere(0.5f, 20, 20));

	mMeshManager->AddMesh("rigidBox", geoGen.CreateBox(1.0f, 1.0f, 1.0f, 0));
}

void Linkup::BuildGameObjects()
{
	auto sky = std::make_shared<Sky>();
	mGameObjectManager->AddGameObject(sky);

	auto cube = std::make_shared<Cube>();
	mGameObjectManager->AddGameObject(cube);

	auto floor = std::make_shared<Floor>();
	mGameObjectManager->AddGameObject(floor);
}

void Linkup::MoveCamera()
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

void Linkup::OnMouseDown(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) != 0) {
		mLastMousePos.x = x;
		mLastMousePos.y = y;
	}
	else if ((btnState & MK_LBUTTON) != 0) {
		Pick(x, y);
	}
}

void Linkup::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void Linkup::OnMouseMove(WPARAM btnState, int x, int y)
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