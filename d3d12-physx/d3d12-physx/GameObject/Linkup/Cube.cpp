#include "LinkupGameObjects.h"

using namespace DirectX;

Cube::Cube(const std::string& name, const Transform& transform)
	:GameObject(name, transform)
{
	// 个人材质
	auto red = std::make_shared<MaterialData>();
	red->DiffuseMapIndex = -1;
	red->NormalMapIndex = GetTextureIndex("tile_nmap");
	red->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	red->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	red->Roughness = 0.1f;
	red->LerpDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	red->LerpPara = 0.0f;
	AddMaterial(name, red);

	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(name, transform);
	mMeshRender->mMatName = name;
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = "box";
	mMeshRender->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRender->mReceiveShadow = true;
	mMeshRender->AddMeshRender();

	//// 刚体
	//Transform rigidDynamicLocal = Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	//mRigidDynamic = std::make_unique<RigidDynamic>(name, transform, rigidDynamicLocal);
	//mRigidDynamic->mScale = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	//mRigidDynamic->mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	//mRigidDynamic->mPxGeometry = PxBoxEnum;
	//mRigidDynamic->mDensity = 10.0f;
	//mRigidDynamic->AddRigidDynamic();

	// 刚体
	Transform rigidStaticLocal = Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	mRigidStatic = std::make_unique<RigidStatic>(name, transform, rigidStaticLocal);
	mRigidStatic->mScale = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	mRigidStatic->mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mRigidStatic->mPxGeometry = PxBoxEnum;
	mRigidStatic->AddRigidStatic();


}

Cube::~Cube()
{
}

void Cube::GetPicked(float dst, DirectX::XMFLOAT3 hitPoint)
{
	std::shared_ptr<Logic> logic = std::dynamic_pointer_cast<Logic>(GetGameObject("Logic"));

	logic->PickCube(mMapIndex);
}

void Cube::TurnOnBlink()
{
	mBlink = true;
}

void Cube::TurnOffBlink()
{
	mBlink = false;
	auto mat = GetMaterial();
	mat->LerpPara = 0.0f;
}

void Cube::SetColor(DirectX::XMFLOAT3 color)
{
	auto mat = GetMaterial();
	mat->DiffuseAlbedo = XMFLOAT4(color.x, color.y, color.z, 1.0f);
}

DirectX::XMFLOAT3 Cube::GetColor()
{
	auto mat = GetMaterial();
	return XMFLOAT3(mat->DiffuseAlbedo.x, mat->DiffuseAlbedo.y, mat->DiffuseAlbedo.z);
}

void Cube::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	if (GetKeyDown('Z')) {
		SwitchScene("MainScene");
	}

	if (GetKeyDown('F')) {
		DeleteGameObject(mName);
		return;
	}

	if (mBlink) {
		auto mat = GetMaterial();

		if (mTurnWhite) {
			mat->LerpPara += gt.DeltaTime() * 3;
			if (mat->LerpPara >= 1) {
				mTurnWhite = false;
			}
		}
		else {
			mat->LerpPara -= gt.DeltaTime() * 3;
			if (mat->LerpPara <= 0) {
				mTurnWhite = true;
			}
		}
	}
}
