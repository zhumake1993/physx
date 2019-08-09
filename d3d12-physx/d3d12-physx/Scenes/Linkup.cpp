#include "Linkup.h"

#include "GameObject/Linkup/LinkupGameObjects.h"

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

	mIsCubeMap = false;
	mIsShadowMap = false;
	mIsSsao = false;
}

void Linkup::Update(const GameTimer& gt)
{
	Scene::Update(gt);

	if (mInputManager->GetMouseDown(0)) {
		Pick(mInputManager->GetMouseX(), mInputManager->GetMouseY());
	}
}

void Linkup::PostUpdate(const GameTimer& gt)
{
	Scene::PostUpdate(gt);
}

void Linkup::BuildManagers()
{
	mTextureManager->Initialize();
	mMaterialManager->Initialize();
	mMeshManager->Initialize();
	mMeshRenderInstanceManager->Initialize();
	mGameObjectManager->Initialize();
	mInputManager->Initialize();
}

void Linkup::BuildTextures()
{
	std::vector<std::wstring> fileNames =
	{
		L"Textures/bricks.dds",
		L"Textures/bricks_nmap.dds",
		L"Textures/bricks2.dds",
		L"Textures/bricks2_nmap.dds",
		L"Textures/checkboard.dds",
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
}

void Linkup::BuildMeshes()
{
	GeometryGenerator geoGen;
	mMeshManager->AddMesh("box", geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3));
	mMeshManager->AddMesh("grid", geoGen.CreateGrid(20.0f, 30.0f, 60, 40));
	mMeshManager->AddMesh("sphere", geoGen.CreateSphere(0.5f, 20, 20));

	mMeshManager->AddMesh("UnitBox", geoGen.CreateBox(1.0f, 1.0f, 1.0f, 0));

	mMeshManager->AddMesh("Segment", geoGen.CreateCylinder(1.0f, 1.0f, 1.0f, 20, 20));
	mMeshManager->AddMesh("Inflection", geoGen.CreateSphere(1.0f, 20, 20));
}

void Linkup::BuildGameObjects()
{
	auto player = std::make_shared<Player>(Transform(XMFLOAT3(0.0f, 2.0f, -10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)), "Player");
	mGameObjectManager->AddGameObject(player);

	auto logic = std::make_shared<Logic>(Transform(XMFLOAT3(0.0f, 2.0f, -15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)), "Logic");
	mGameObjectManager->AddGameObject(logic);

	auto renderAndFilter = std::make_shared<RenderAndFilter>(Transform(), "RenderAndFilter");
	mGameObjectManager->AddGameObject(renderAndFilter);

	auto sky = std::make_shared<Sky>(Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT3(5000.0f, 5000.0f, 5000.0f)), "Sky");
	mGameObjectManager->AddGameObject(sky);

	auto floor = std::make_shared<Floor>(Transform(), "Floor");
	mGameObjectManager->AddGameObject(floor);
}