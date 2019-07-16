#include "Cylinder.h"

Cylinder::Cylinder()
	:GameObject()
{
	mMatName = "bricks";
	XMStoreFloat4x4(&mTexTransform, XMMatrixScaling(1.5f, 2.0f, 1.0f));

	mMeshName = "cylinder";

	mRenderLayer = (int)RenderLayer::Opaque;
}

Cylinder::~Cylinder()
{
}

void Cylinder::Update()
{
}
