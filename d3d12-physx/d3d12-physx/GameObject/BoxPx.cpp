#include "BoxPx.h"

BoxPx::BoxPx()
	:GameObject()
{
	mGameObjectName = "boxPx";

	mTranslation = XMFLOAT3(0.0f, 10.0f, -10.0f);

	mMatName = "bricks2";
	XMStoreFloat4x4(&mTexTransform, XMMatrixScaling(1.0f, 0.5f, 1.0f));

	mMeshName = "box";

	mRenderLayer = (int)RenderLayer::Opaque;

	//======
	// PhysX
	//======
	mPxLocalPos = XMFLOAT3(0.0f, 1.0f, 0.0f);;
	mPxLocalQuat = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mPxGeometry = PxBox;
	PxBoxGeometryDesc boxDesc;
	boxDesc.hx = 0.5f;
	boxDesc.hy = 0.5f;
	boxDesc.hz = 0.5f;
	mPxGeometryDesc = &boxDesc;
	mDensity = 10.0f;
	AddRigidBody();
}

BoxPx::~BoxPx()
{
}

void BoxPx::Update()
{
	GameObject::Update();
}
