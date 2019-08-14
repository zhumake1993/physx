#include "LinkupGameObjects.h"

using namespace DirectX;

Cube::Cube(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	mLayer = "Cube";
	mIsStatic = true;

	// Material
	mMaterial = std::make_shared<Material>();
	mMaterial->mDiffuseMapIndex = -1;
	mMaterial->mNormalMapIndex = GetTextureIndex("tile_nmap");
	mMaterial->mDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterial->mFresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	mMaterial->mRoughness = 0.1f;
	mMaterial->mLerpDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterial->mLerpPara = 0.0f;
	AddMaterial();

	// MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(transform);
	mMeshRenderCPT->mMaterial = mMaterial;
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRenderCPT->mMeshName = "UnitBox";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRenderCPT->mReceiveShadow = true;
	mMeshRenderCPT->mParent = mName;
	mMeshRenderCPT->AddMeshRender();

	// Rigidbody
	Transform rigidStaticLocal = Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	mRigidStaticCPT = std::make_shared<RigidStaticCPT>(mName, transform, rigidStaticLocal);
	mRigidStaticCPT->mScale = XMFLOAT4(0.48f, 0.48f, 0.48f, 0.48f);
	mRigidStaticCPT->mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mRigidStaticCPT->mPxGeometry = PxBoxEnum;
	mRigidStaticCPT->AddRigidStatic();
}

Cube::~Cube()
{
}

void Cube::GetPicked(const float& dst, const DirectX::XMFLOAT3& hitPoint)
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
	mat->mLerpPara = 0.0f;
	SetMaterial(mat);
}

void Cube::SetColor(DirectX::XMFLOAT3 color)
{
	auto mat = GetMaterial();
	mat->mDiffuseAlbedo = XMFLOAT4(color.x, color.y, color.z, 1.0f);
	SetMaterial(mat);
}

DirectX::XMFLOAT3 Cube::GetColor()
{
	auto mat = GetMaterial();
	return XMFLOAT3(mat->mDiffuseAlbedo.x, mat->mDiffuseAlbedo.y, mat->mDiffuseAlbedo.z);
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
			mat->mLerpPara += gt.DeltaTime() * 3;
			if (mat->mLerpPara >= 1) {
				mTurnWhite = false;
			}
		}
		else {
			mat->mLerpPara -= gt.DeltaTime() * 3;
			if (mat->mLerpPara <= 0) {
				mTurnWhite = true;
			}
		}

		SetMaterial(mat);
	}
}

void Cube::Release()
{
	GameObject::Release();
}
