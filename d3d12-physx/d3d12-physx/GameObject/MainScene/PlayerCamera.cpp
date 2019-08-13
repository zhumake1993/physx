#include "PlayerCamera.h"

using namespace DirectX;

PlayerCamera::PlayerCamera(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	mIsStatic = false;

	// Camera
	mCameraCPT = std::make_shared<CameraCPT>(transform);
	mCameraCPT->SetLens(0.25f * MathHelper::Pi, 1200.0f / 900.0f, 1.0f, 1000.0f);
	mCameraCPT->SetFrustumCulling(false);
	mCameraCPT->SetMainCamera();
}

PlayerCamera::~PlayerCamera()
{
}

void PlayerCamera::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	Move(gt);
}

void PlayerCamera::Move(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	if (GetMouseDown(1)) {
		mLastMousePos.x = GetMouseX();
		mLastMousePos.y = GetMouseY();
	}

	if (GetMousePress(1)) {
		// ÿ���ض�Ӧ0.25��
		float dx = XMConvertToRadians(0.25f * static_cast<float>(GetMouseX() - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(GetMouseY() - mLastMousePos.y));

		Pitch(dy);
		RotateY(dx);

		mLastMousePos.x = GetMouseX();
		mLastMousePos.y = GetMouseY();
	}

	if (GetKeyPress('W'))Walk(10.0f * dt);
	if (GetKeyPress('S'))Walk(-10.0f * dt);
	if (GetKeyPress('A'))Strafe(-10.0f * dt);
	if (GetKeyPress('D'))Strafe(10.0f * dt);
	if (GetKeyPress('Q'))Fly(10.0f * dt);
	if (GetKeyPress('E'))Fly(-10.0f * dt);
}

void PlayerCamera::Pitch(float angle)
{
	XMVECTOR quat = XMLoadFloat4(&mTransform.Quaternion);
	auto mat = XMMatrixRotationQuaternion(quat);

	auto R = XMQuaternionRotationAxis(mat.r[0], angle);
	XMStoreFloat4(&mTransform.Quaternion, XMQuaternionMultiply(quat, R));
}

void PlayerCamera::RotateY(float angle)
{
	auto R = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), angle);
	XMVECTOR quat = XMLoadFloat4(&mTransform.Quaternion);
	XMStoreFloat4(&mTransform.Quaternion, XMQuaternionMultiply(quat, R));
}

void PlayerCamera::Strafe(float d)
{
	XMVECTOR quat = XMLoadFloat4(&mTransform.Quaternion);
	auto mat = XMMatrixRotationQuaternion(quat);

	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR p = XMLoadFloat3(&mTransform.Translation);
	XMStoreFloat3(&mTransform.Translation, XMVectorMultiplyAdd(s, mat.r[0], p));
}

void PlayerCamera::Walk(float d)
{
	XMVECTOR quat = XMLoadFloat4(&mTransform.Quaternion);
	auto mat = XMMatrixRotationQuaternion(quat);

	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR p = XMLoadFloat3(&mTransform.Translation);
	XMStoreFloat3(&mTransform.Translation, XMVectorMultiplyAdd(s, mat.r[2], p));
}

void PlayerCamera::Fly(float d)
{
	mTransform.Translation.y += d;
}