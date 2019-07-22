#include "Grid.h"

using namespace DirectX;

Grid::Grid()
	:GameObject()
{
	// 基础信息
	mName = "grid";

	// 添加MeshRender
	mHasMeshRender = true;
	mMeshRender.MatName = "tile";
	XMStoreFloat4x4(&mMeshRender.TexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
	mMeshRender.MeshName = "grid";
}

Grid::~Grid()
{
}

void Grid::Update()
{
	GameObject::Update();
}
