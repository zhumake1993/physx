#include "CameraCPT.h"
#include "Manager/SceneManager.h"

using namespace DirectX;

CameraCPT::CameraCPT(const Transform& transform)
{
	mCamera = std::make_shared<Camera>();
	SetPosition(transform.Translation);
	SetQuaterion(transform.Quaternion);
}

CameraCPT::~CameraCPT()
{
}

void CameraCPT::SetMainCamera()
{
	SetCurrMainCamera(mCamera);
}

void CameraCPT::SetPosition(float x, float y, float z)
{
	mCamera->SetPosition(x, y, z);
}

void CameraCPT::SetPosition(const DirectX::XMFLOAT3& v)
{
	mCamera->SetPosition(v);
}

void CameraCPT::SetQuaterion(const DirectX::XMFLOAT4& q)
{
	XMVECTOR quat = XMLoadFloat4(&q);
	auto mat = XMMatrixRotationQuaternion(quat);

	XMVECTOR pos = mCamera->GetPosition();
	XMVECTOR target = pos + mat.r[2];
	XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	mCamera->LookAt(pos, target, worldUp);
}

void CameraCPT::SetLens(float fovY, float aspect, float zn, float zf)
{
	mCamera->SetLens(fovY, aspect, zn, zf);
}

void CameraCPT::SetFrustumCulling(bool st)
{
	mCamera->mFrustumCullingEnabled = st;
}

void CameraCPT::Update()
{
}
