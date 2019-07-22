#include "Skull.h"

using namespace DirectX;

#include "Common/GameTimer.h"
extern GameTimer gTimer;

Skull::Skull()
	:GameObject()
{
	// 基础信息
	mName = "skull";
	mTransform.Scale = XMFLOAT3(0.2f, 0.2f, 0.2f);

	// 添加MeshRender
	mHasMeshRender = true;
	mMeshRender.MatName = "skullMat";
	mMeshRender.MeshName = "skull";
}

Skull::~Skull()
{
}

void Skull::Update()
{
	GameObject::Update();

	mTransform.Translation = XMFLOAT3(3.0f * cos(gTimer.TotalTime()*0.5f), 2.0f, 3.0f * sin(gTimer.TotalTime()*0.5f));
	XMStoreFloat4(&mTransform.Quaternion, XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), gTimer.TotalTime()));
}
