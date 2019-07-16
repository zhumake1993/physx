#include "Box.h"

Box::Box()
	:GameObject()
{
	mGameObjectName = "box";

	mTranslation = XMFLOAT3(0.0f, 0.5f, 0.0f);
	mScale = XMFLOAT3(2.0f, 1.0f, 2.0f);

	mMatName = "bricks2";
	XMStoreFloat4x4(&mTexTransform, XMMatrixScaling(1.0f, 0.5f, 1.0f));

	mMeshName = "box";

	mRenderLayer = (int)RenderLayer::Opaque;
}

Box::~Box()
{
}

void Box::Update()
{
}
