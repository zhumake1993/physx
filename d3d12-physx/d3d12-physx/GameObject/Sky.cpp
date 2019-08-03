#include "Sky.h"

using namespace DirectX;

Sky::Sky(const Transform& transform)
	:GameObject(transform)
{
	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(transform);
	mMeshRender->mMatName = "sky";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = "sphere";
	mMeshRender->mRenderLayer = (int)RenderLayer::Sky;
	mMeshRender->mReceiveShadow = false;
	mMeshRender->AddMeshRender();
}

Sky::~Sky()
{
}

void Sky::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
