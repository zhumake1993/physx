#include "Grid.h"

Grid::Grid()
	:GameObject()
{
	mGameObjectName = "grid";

	mMatName = "tile";
	XMStoreFloat4x4(&mTexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));

	mMeshName = "grid";

	mRenderLayer = (int)RenderLayer::Opaque;
}

Grid::~Grid()
{
}

void Grid::Update()
{
}
