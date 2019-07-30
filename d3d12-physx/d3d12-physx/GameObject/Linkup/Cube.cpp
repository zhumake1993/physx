#include "LinkupGameObjects.h"

using namespace DirectX;

Cube::Cube(const std::string& name, const Transform& transform)
	:GameObject(name, transform)
{
	// 个人材质
	auto red = std::make_shared<MaterialData>();
	red->DiffuseMapIndex = -1;
	red->NormalMapIndex = -1;
	red->DiffuseAlbedo = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
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

	// 刚体
	Transform rigidDynamicLocal = Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	mRigidDynamic = std::make_unique<RigidDynamic>(name, transform, rigidDynamicLocal);
	mRigidDynamic->mScale = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	mRigidDynamic->mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mRigidDynamic->mPxGeometry = PxBoxEnum;
	mRigidDynamic->mDensity = 10.0f;
	mRigidDynamic->AddRigidDynamic();

	// 刚体
	//Transform rigidStaticLocal = Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	//mRigidStatic = std::make_unique<RigidStatic>(name, transform, rigidStaticLocal);
	//mRigidStatic->mScale = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	//mRigidStatic->mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	//mRigidStatic->mPxGeometry = PxBoxEnum;
	//mRigidStatic->AddRigidStatic();


}

Cube::~Cube()
{
}

void Cube::GetPicked(float dst, DirectX::XMFLOAT3 hitPoint)
{
	std::shared_ptr<Logic> last = std::dynamic_pointer_cast<Logic>(GetGameObject("Logic"));

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

		if (HasGameObject("CubeGreen")) {
			auto other = std::dynamic_pointer_cast<CubeGreen>(GetGameObject("CubeGreen"));
			other->TurnOffBlink();
		}
	}
}

void Cube::TurnOffBlink()
{
	mBlink = false;
	auto mat = GetMaterial();
	mat->LerpPara = 0.0f;
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

	if (GetKeyDown('C')) {
		mRigidDynamic->SetAngularDamping(0.5f);
		mRigidDynamic->SetLinearVelocity(XMFLOAT3(0.0f, 10.5f, 0.0f));
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
