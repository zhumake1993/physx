#include "BoxPx.h"

BoxPx::BoxPx()
	:GameObject()
{
	mGameObjectName = "boxPx";

	mTranslation = XMFLOAT3(0.0f, 10.0f, 0.0f);

	mMatName = "bricks2";
	XMStoreFloat4x4(&mTexTransform, XMMatrixScaling(1.0f, 0.5f, 1.0f));

	mMeshName = "box";

	mRenderLayer = (int)RenderLayer::Opaque;
}

BoxPx::~BoxPx()
{
}

void BoxPx::Update()
{
}
