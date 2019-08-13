#include "CameraCPT.h"
#include "Manager/SceneManager.h"

using namespace DirectX;

CameraCPT::CameraCPT(const Transform& transform)
{
	mCamera = std::make_shared<Camera>();
	Update(transform);
}

CameraCPT::~CameraCPT()
{
}

void CameraCPT::SetMainCamera()
{
	SetCurrMainCamera(mCamera);
}

void CameraCPT::SetTranslation(float x, float y, float z)
{
	mCamera->SetTranslation(x, y, z);
}

void CameraCPT::SetTranslation(const DirectX::XMFLOAT3& v)
{
	mCamera->SetTranslation(v);
}

void CameraCPT::SetQuaterion(float x, float y, float z, float w)
{
	mCamera->SetQuaternion(x, y, z, w);
}

void CameraCPT::SetQuaterion(const DirectX::XMFLOAT4& q)
{
	mCamera->SetQuaternion(q);
}

void CameraCPT::SetLens(float fovY, float aspect, float zn, float zf)
{
	mCamera->SetLens(fovY, aspect, zn, zf);
}

void CameraCPT::SetFrustumCulling(bool st)
{
	mCamera->SetFrustumCulling(st);
}

void CameraCPT::Update(const Transform& transform)
{
	SetTranslation(transform.Translation);
	SetQuaterion(transform.Quaternion);
}
