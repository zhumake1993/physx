#include "LinkupGameObjects.h"

using namespace DirectX;

CubeGreen::CubeGreen(const std::string& name, const Transform& transform)
	:GameObject(name, transform)
{
	// 个人材质
	auto green = std::make_shared<MaterialData>();
	green->DiffuseMapIndex = -1;
	green->NormalMapIndex = -1;
	green->DiffuseAlbedo = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	green->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	green->Roughness = 0.1f;
	green->LerpDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	green->LerpPara = 0.0f;
	AddMaterial(name, green);

	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(name, transform);
	mMeshRender->mMatName = name;
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = "box";
	mMeshRender->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRender->mReceiveShadow = true;
	mMeshRender->AddMeshRender();

	// 刚体
	Transform rigidStaticLocal = Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	mRigidStatic = std::make_unique<RigidStatic>(name, transform, rigidStaticLocal);
	mRigidStatic->mScale = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	mRigidStatic->mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mRigidStatic->mPxGeometry = PxBoxEnum;
	mRigidStatic->AddRigidStatic();
}

CubeGreen::~CubeGreen()
{
}

void CubeGreen::GetPicked(float dst, DirectX::XMFLOAT3 hitPoint)
{
	auto last = std::dynamic_pointer_cast<Logic>(GetGameObject("Logic"));

	if (last->mLastPickCube == "") {
		mBlink = true;

		last->mLastPickCube = mName;
	}
	else if (last->mLastPickCube == mName) {
		mBlink = false;

		last->mLastPickCube = "";

		TurnOffBlink();
	}
	else {
		mBlink = false;

		last->mLastPickCube = "";

		TurnOffBlink();

		if (HasGameObject("Cube")) {
			auto other = std::dynamic_pointer_cast<Cube>(GetGameObject("Cube"));
			other->TurnOffBlink();
		}
	}
}

void CubeGreen::TurnOffBlink()
{
	mBlink = false;
	auto mat = GetMaterial();
	mat->LerpPara = 0.0f;
}

void CubeGreen::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	if (GetKeyDown('Z')) {
		SwitchScene("MainScene");
	}

	if (mBlink) {
		auto mat = GetMaterial();

		if (mTurnWhite) {
			mat->LerpPara += gt.DeltaTime();
			if (mat->LerpPara >= 1) {
				mTurnWhite = false;
			}
		}
		else {
			mat->LerpPara -= gt.DeltaTime();
			if (mat->LerpPara <= 0) {
				mTurnWhite = true;
			}
		}
	}
}
