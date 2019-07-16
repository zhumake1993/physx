#include "Sky.h"

Sky::Sky()
	:GameObject()
{
	mGameObjectName = "sky";

	mScale = XMFLOAT3(5000.0f, 5000.0f, 5000.0f);

	mMatName = "sky";

	mMeshName = "sphere";

	mRenderLayer = (int)RenderLayer::Sky;
}

Sky::~Sky()
{
}

void Sky::Update()
{
}
