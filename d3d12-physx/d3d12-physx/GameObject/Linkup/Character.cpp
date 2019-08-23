#include "LinkupGameObjects.h"

using namespace DirectX;

Character::Character(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	mIsStatic = false;

	// ��ɫ������
	mCharacterControllerCPT = std::make_shared<CharacterControllerCPT>(mName, transform);
	mCharacterControllerCPT->mContactOffset = 0.01f;
	mCharacterControllerCPT->mStepOffset = 0.1f;
	mCharacterControllerCPT->mSlopeLimit= cosf(XM_PIDIV4);
	mCharacterControllerCPT->mRadius = 0.25f;
	mCharacterControllerCPT->mHeight = 1.0f;
	mCharacterControllerCPT->mUpDirection = XMFLOAT3(0.0f, 1.0f, 0.0f);
	mCharacterControllerCPT->AddCharacterController();

	// Camera
	mCameraCPT = std::make_shared<CameraCPT>(transform);
	mCameraCPT->SetLens(0.25f * MathHelper::Pi, 1200.0f / 900.0f, 0.1f, 1000.0f);
	mCameraCPT->SetFrustumCulling(false);
	mCameraCPT->SetMainCamera();
}

Character::~Character()
{
}

void Character::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	const auto delta = gt.DeltaTime();

	// ����
	float verticalDist = mCurrVerticalSpeed * delta;

	if (mCharacterControllerCPT->Move(XMFLOAT3(0.0f, verticalDist, 0.0f), 0.01f, delta) == 3) {
		mFallTime = 0.0f;
		mStartVerticalSpeed = 0.0f;
	}

	mCurrVerticalSpeed = mStartVerticalSpeed + mGravity * mFallTime;
	mFallTime += delta;

	// �ƶ�
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

	// �ӽ�
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

	// ѡȡ
	if (GetMouseDown(0)) {
		Pick(GetMouseX(), GetMouseY());
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

void Character::Pick(int sx, int sy)
{
	XMFLOAT4X4 P = GetMainCamera()->GetProj4x4f();

	// �����ӿռ��ѡȡ����
	float vx = (+2.0f * sx / GetClientWidth() - 1.0f) / P(0, 0);
	float vy = (-2.0f * sy / GetClientHeight() + 1.0f) / P(1, 1);

	// �ӿռ�����߶���
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	// ������ת��������ռ�
	XMMATRIX V = GetMainCamera()->GetView();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	XMVECTOR rayOriginW = XMVector3TransformCoord(rayOrigin, invView);
	XMVECTOR rayDirW = XMVector3TransformNormal(rayDir, invView);

	XMFLOAT3 origin;
	XMFLOAT3 direction;
	XMStoreFloat3(&origin, rayOriginW);
	XMStoreFloat3(&direction, rayDirW);

	auto hits = Raycast(origin, direction);

	for (auto& h : hits) {
		auto gameobject = GetGameObject(h.Name);
		if (gameobject->GetLayer() == "Cube") {
			std::shared_ptr<Cube> cube = std::dynamic_pointer_cast<Cube>(gameobject);
			cube->GetPicked(h.Dist, h.Point);
			break;
		}
	}
}