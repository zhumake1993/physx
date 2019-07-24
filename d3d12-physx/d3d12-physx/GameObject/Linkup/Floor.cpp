#include "Floor.h"

using namespace DirectX;

Floor::Floor()
	:GameObject()
{
	// 基础信息
	mName = "Floor";

	// 添加MeshRender
	mHasMeshRender = true;
	mMeshRender.MatName = "tile";
	XMStoreFloat4x4(&mMeshRender.TexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
	mMeshRender.MeshName = "grid";
}

Floor::~Floor()
{
}

void Floor::Update()
{
	GameObject::Update();
}
