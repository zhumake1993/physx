#include "Box.h"

using namespace DirectX;

Box::Box(const Transform& transform)
	:GameObject(transform)
{
	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(transform);
	mMeshRender->mMatName = "bricks2";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 0.5f, 1.0f));
	mMeshRender->mMeshName = "box";
	mMeshRender->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRender->mReceiveShadow = true;
	mMeshRender->AddMeshRender();
}

Box::~Box()
{
}

void Box::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
