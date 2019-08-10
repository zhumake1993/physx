#include "LinkupGameObjects.h"

using namespace DirectX;

Player::Player(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	mIsStatic = false;

	// Camera
	mCameraCPT = std::make_shared<CameraCPT>(transform);
	mCameraCPT->SetLens(0.25f * MathHelper::Pi, 1200.0f / 900.0f, 1.0f, 1000.0f);
	mCameraCPT->SetFrustumCulling(false);
	mCameraCPT->SetMainCamera();

	// 刚体
	Transform rigidDynamicLocal = Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	mRigidDynamicCPT = std::make_shared<RigidDynamicCPT>(mName, transform, rigidDynamicLocal);
	mRigidDynamicCPT->mScale = XMFLOAT4(1.48f, 1.48f, 1.48f, 0.48f);
	mRigidDynamicCPT->mPxMaterial = XMFLOAT3(3.4e+38F, 3.4e+38F, 0.0f);
	mRigidDynamicCPT->mPxGeometry = PxBoxEnum;
	mRigidDynamicCPT->mDensity = 1.0f;
	mRigidDynamicCPT->isKinematic = false;
	mRigidDynamicCPT->AddRigidDynamic();

	SetRigidDynamicLockFlag(3, true);
	SetRigidDynamicLockFlag(5, true);
}

Player::~Player()
{
}

void Player::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	//Move(gt);
}

void Player::Move(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	const float forcePara = 1000.0f;

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

	if (GetKeyPress('W')) {
		auto dir = mTransform.GetForward();
		dir.y = 0.0f;
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = XMVector3Normalize(dirV) * forcePara;
		XMStoreFloat3(&dir, dirV);
		AddForce(dir);
	}

	if (GetKeyPress('S')) {
		auto dir = mTransform.GetForward();
		dir.y = 0.0f;
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = -XMVector3Normalize(dirV) * forcePara;
		XMStoreFloat3(&dir, dirV);
		AddForce(dir);
	}

	if (GetKeyPress('A')) {
		auto dir = mTransform.GetRight();
		dir.y = 0.0f;
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = -XMVector3Normalize(dirV) * forcePara;
		XMStoreFloat3(&dir, dirV);
		AddForce(dir);
	}

	if (GetKeyPress('D')) {
		auto dir = mTransform.GetRight();
		dir.y = 0.0f;
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = XMVector3Normalize(dirV) * forcePara;
		XMStoreFloat3(&dir, dirV);
		AddForce(dir);
	}

	if (GetKeyPress(VK_SPACE)) {
		auto dir = mTransform.GetUp();
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = XMVector3Normalize(dirV) * forcePara;
		XMStoreFloat3(&dir, dirV);
		AddForce(dir);
	}
}

void Player::Pitch(float angle)
{
	XMVECTOR quat = XMLoadFloat4(&mTransform.Quaternion);
	auto mat = XMMatrixRotationQuaternion(quat);

	auto R = XMQuaternionRotationAxis(mat.r[0], angle);
	XMStoreFloat4(&mTransform.Quaternion, XMQuaternionMultiply(quat, R));
}

void Player::RotateY(float angle)
{
	auto R = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), angle);
	XMVECTOR quat = XMLoadFloat4(&mTransform.Quaternion);
	XMStoreFloat4(&mTransform.Quaternion, XMQuaternionMultiply(quat, R));
}