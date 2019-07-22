#include "Box.h"

using namespace DirectX;

Box::Box()
	:GameObject()
{
	// 基础信息
	mName = "box";
	mTransform.Translation = XMFLOAT3(0.0f, 0.5f, 0.0f);
	mTransform.Scale = XMFLOAT3(2.0f, 1.0f, 2.0f);

	// 添加MeshRender
	mHasMeshRender = true;
	mMeshRender.MatName = "bricks2";
	XMStoreFloat4x4(&mMeshRender.TexTransform, XMMatrixScaling(1.0f, 0.5f, 1.0f));
	mMeshRender.MeshName = "box";
}

Box::~Box()
{
}

void Box::Update()
{
	GameObject::Update();
}
