#include "LinkupGameObjects.h"

using namespace DirectX;

Test::Test(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	mIsStatic = false;

	//// Material
	//mMaterial = std::make_shared<Material>();
	//mMaterial->mDiffuseMapIndex = -1;
	//mMaterial->mNormalMapIndex = GetTextureIndex("tile_nmap");
	//mMaterial->mDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//mMaterial->mFresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	//mMaterial->mRoughness = 0.1f;
	//mMaterial->mLerpDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//mMaterial->mLerpPara = 0.0f;
	//AddMaterial();

	//// MeshRender
	//mMeshRenderCPT = std::make_shared<MeshRenderCPT>(transform);
	//mMeshRenderCPT->mMaterial = mMaterial;
	//XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	//mMeshRenderCPT->mMeshName = "box";
	//mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Opaque;
	//mMeshRenderCPT->mReceiveShadow = true;
	//mMeshRenderCPT->mParent = mName;
	//mMeshRenderCPT->AddMeshRender();

	// 刚体
	auto rotation = XMQuaternionRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XM_PIDIV2);
	XMFLOAT4 quat;
	XMStoreFloat4(&quat, rotation);
	Transform rigidDynamicLocal = Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), quat);
	mRigidDynamicCPT = std::make_shared<RigidDynamicCPT>(transform, rigidDynamicLocal);
	mRigidDynamicCPT->mScale = XMFLOAT4(0.48f, 0.48f, 0.48f, 0.48f);
	mRigidDynamicCPT->mPxMaterial = XMFLOAT3(3.4e+38F, 0, 0.0f);
	mRigidDynamicCPT->mPxGeometry = PxCapsuleEnum; // PxCapsuleEnum PxBoxEnum
	mRigidDynamicCPT->mDensity = 1.0f;
	mRigidDynamicCPT->isKinematic = false;
	mRigidDynamicCPT->AddRigidDynamic();

	//SetRigidDynamicLockFlag(3, true);
	//SetRigidDynamicLockFlag(5, true);
}

Test::~Test()
{
}

void Test::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	Move(gt);
}

void Test::Move(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();
	const float forcePara = 30.0f;

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
		auto dir = GetForward();
		dir.y = 0.0f;
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = XMVector3Normalize(dirV) * forcePara;
		XMStoreFloat3(&dir, dirV);
		AddForce(dir);
	}

	if (GetKeyPress('S')) {
		auto dir = GetForward();
		dir.y = 0.0f;
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = -XMVector3Normalize(dirV) * forcePara;
		XMStoreFloat3(&dir, dirV);
		AddForce(dir);
	}

	if (GetKeyPress('A')) {
		auto dir = GetRight();
		dir.y = 0.0f;
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = -XMVector3Normalize(dirV) * forcePara;
		XMStoreFloat3(&dir, dirV);
		AddForce(dir);
	}

	if (GetKeyPress('D')) {
		auto dir = GetRight();
		dir.y = 0.0f;
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = XMVector3Normalize(dirV) * forcePara;
		XMStoreFloat3(&dir, dirV);
		AddForce(dir);
	}

	if (GetKeyPress(VK_SPACE)) {
		auto dir = GetUp();
		XMVECTOR dirV = XMLoadFloat3(&dir);
		dirV = XMVector3Normalize(dirV) * forcePara;
		XMStoreFloat3(&dir, dirV);
		AddForce(dir);
	}
}

void Test::Pitch(float angle)
{
	XMVECTOR quat = XMLoadFloat4(&mTransform.Quaternion);
	auto mat = XMMatrixRotationQuaternion(quat);

	auto R = XMQuaternionRotationAxis(mat.r[0], angle);
	XMStoreFloat4(&mTransform.Quaternion, XMQuaternionMultiply(quat, R));
}

void Test::RotateY(float angle)
{
	auto R = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), angle);
	XMVECTOR quat = XMLoadFloat4(&mTransform.Quaternion);
	XMStoreFloat4(&mTransform.Quaternion, XMQuaternionMultiply(quat, R));
}