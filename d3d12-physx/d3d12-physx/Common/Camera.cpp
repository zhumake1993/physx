#include "Camera.h"

using namespace DirectX;

Camera::Camera()
{
}

Camera::~Camera()
{
}

XMVECTOR Camera::GetTranslation()const { return XMLoadFloat3(&mTranslation); }
XMFLOAT3 Camera::GetTranslation3f()const { return mTranslation; }
void Camera::SetTranslation(float x, float y, float z) { mTranslation = XMFLOAT3(x, y, z); UpdateViewMatrix(); }
void Camera::SetTranslation(const XMFLOAT3& v) { mTranslation = v; UpdateViewMatrix(); }

XMVECTOR Camera::GetQuaternion() const { return XMLoadFloat4(&mQuaternion); }
XMFLOAT4 Camera::GetQuaternion4f() const { return mQuaternion; }
void Camera::SetQuaternion(float x, float y, float z, float w) { mQuaternion = XMFLOAT4(x, y, z, w); UpdateViewMatrix(); }
void Camera::SetQuaternion(const XMFLOAT4& v) { mQuaternion = v; UpdateViewMatrix(); }

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);

	BoundingFrustum::CreateFromMatrix(mCamFrustum, GetProj());
}

BoundingFrustum Camera::GetCamFrustum()
{
	return mCamFrustum;
}

bool Camera::GetFrustumCulling()
{
	return mFrustumCullingEnabled;
}

void Camera::SetFrustumCulling(bool st)
{
	mFrustumCullingEnabled = st;
}

float Camera::GetNearZ()const { return mNearZ; }
float Camera::GetFarZ()const { return mFarZ; }
float Camera::GetAspect()const { return mAspect; }
float Camera::GetFovY()const { return mFovY; }
float Camera::GetFovX()const { float halfWidth = 0.5f * GetNearWindowWidth(); return 2.0f * atan(halfWidth / mNearZ); }

float Camera::GetNearWindowWidth()const { return mAspect * mNearWindowHeight; }
float Camera::GetNearWindowHeight()const { return mNearWindowHeight; }
float Camera::GetFarWindowWidth()const { return mAspect * mFarWindowHeight; }
float Camera::GetFarWindowHeight()const { return mFarWindowHeight; }

XMMATRIX Camera::GetView()const { return XMLoadFloat4x4(&mView); }
XMMATRIX Camera::GetProj()const { return XMLoadFloat4x4(&mProj); }
XMFLOAT4X4 Camera::GetView4x4f()const { return mView; }
XMFLOAT4X4 Camera::GetProj4x4f()const { return mProj; }

void Camera::UpdateViewMatrix()
{
	XMVECTOR tran = XMLoadFloat3(&mTranslation);
	XMVECTOR quat = XMLoadFloat4(&mQuaternion);
	auto mat = XMMatrixRotationQuaternion(quat);

	XMStoreFloat4x4(&mView, XMMatrixTranspose(mat));

	mView(3, 0) = -XMVectorGetX(XMVector3Dot(tran, mat.r[0]));
	mView(3, 1) = -XMVectorGetX(XMVector3Dot(tran, mat.r[1]));
	mView(3, 2) = -XMVectorGetX(XMVector3Dot(tran, mat.r[2]));
}


