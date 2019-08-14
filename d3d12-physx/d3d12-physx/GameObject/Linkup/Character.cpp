#include "LinkupGameObjects.h"

using namespace DirectX;

Character::Character(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	mIsStatic = false;

	// MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(transform);
	mMeshRenderCPT->mMaterial = GetDefaultMaterial();
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRenderCPT->mMeshName = "Test";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRenderCPT->mReceiveShadow = true;
	mMeshRenderCPT->mParent = mName;
	mMeshRenderCPT->AddMeshRender();

	// 角色控制器
	mCharacterControllerCPT = std::make_shared<CharacterControllerCPT>(mName, transform);
	mCharacterControllerCPT->mContactOffset = 0.05f;
	mCharacterControllerCPT->mStepOffset = 0.1f;
	mCharacterControllerCPT->mSlopeLimit= cosf(XM_PIDIV4);
	mCharacterControllerCPT->mRadius = 0.5f;
	mCharacterControllerCPT->mHeight = 1.0f;
	mCharacterControllerCPT->mUpDirection = XMFLOAT3(0.0f, 1.0f, 0.0f);
	mCharacterControllerCPT->AddCharacterController();

	// Camera
	mCameraCPT = std::make_shared<CameraCPT>(transform);
	mCameraCPT->SetLens(0.25f * MathHelper::Pi, 1200.0f / 900.0f, 1.0f, 1000.0f);
	mCameraCPT->SetFrustumCulling(false);
	mCameraCPT->SetMainCamera();
}

Character::~Character()
{
}

void Character::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	Move(gt);
}

void Character::Move(const GameTimer& gt)
{
	const auto delta = gt.DeltaTime();
	
	float verticalDist = mCurrVerticalSpeed * delta;

	// 重力
	if (mCharacterControllerCPT->Move(XMFLOAT3(0.0f, verticalDist, 0.0f), 0.01f, delta) == 3) {
		mFallTime = 0.0f;
		mStartVerticalSpeed = 0.0f;
	}

	mCurrVerticalSpeed = mStartVerticalSpeed + mGravity * mFallTime;
	mFallTime += delta;

	if (GetKeyPress('W')) {
		auto dir = mTransform.GetForward();
		dir.y = 0.0f;
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = XMVector3Normalize(dirV) * mLateralSpeed * delta;
		XMStoreFloat3(&dir, dirV);
		mCharacterControllerCPT->Move(dir, 0.01f, delta);
	}

	if (GetKeyPress('S')) {
		auto dir = mTransform.GetForward();
		dir.y = 0.0f;
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = -XMVector3Normalize(dirV) * mLateralSpeed * delta;
		XMStoreFloat3(&dir, dirV);
		mCharacterControllerCPT->Move(dir, 0.01f, delta);
	}

	if (GetKeyPress('A')) {
		auto dir = mTransform.GetRight();
		dir.y = 0.0f;
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = -XMVector3Normalize(dirV) * mLateralSpeed * delta;
		XMStoreFloat3(&dir, dirV);
		mCharacterControllerCPT->Move(dir, 0.01f, delta);
	}

	if (GetKeyPress('D')) {
		auto dir = mTransform.GetRight();
		dir.y = 0.0f;
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = XMVector3Normalize(dirV) * mLateralSpeed * delta;
		XMStoreFloat3(&dir, dirV);
		mCharacterControllerCPT->Move(dir, 0.01f, delta);
	}

	if (GetKeyDown(VK_SPACE)) {
		mStartVerticalSpeed = mVerticalSpeed;
	}

	// 视角
	if (GetMouseDown(1)) {
		mLastMousePos.x = GetMouseX();
		mLastMousePos.y = GetMouseY();
	}

	if (GetMousePress(1)) {
		// 每像素对应0.25度
		float dx = XMConvertToRadians(0.25f * static_cast<float>(GetMouseX() - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(GetMouseY() - mLastMousePos.y));

		Pitch(dy);
		RotateY(dx);

		mLastMousePos.x = GetMouseX();
		mLastMousePos.y = GetMouseY();
	}
}

void Character::Pitch(float angle)
{
	XMVECTOR quat = XMLoadFloat4(&mTransform.Quaternion);
	auto mat = XMMatrixRotationQuaternion(quat);

	auto R = XMQuaternionRotationAxis(mat.r[0], angle);
	XMStoreFloat4(&mTransform.Quaternion, XMQuaternionMultiply(quat, R));
}

void Character::RotateY(float angle)
{
	auto R = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), angle);
	XMVECTOR quat = XMLoadFloat4(&mTransform.Quaternion);
	XMStoreFloat4(&mTransform.Quaternion, XMQuaternionMultiply(quat, R));
}