#include "WirefenceBox.h"

WirefenceBox::WirefenceBox()
	:GameObject()
{
	mGameObjectName = "wirefenceBox";

	mTranslation = XMFLOAT3(3.0f, -18.0f, -9.0f);

	mMatName = "wirefence";

	mMeshName = "box2";

	mRenderLayer = (int)RenderLayer::AlphaTested;
}

WirefenceBox::~WirefenceBox()
{
}

void WirefenceBox::Update()
{
}
