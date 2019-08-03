#include "Globe.h"

using namespace DirectX;

Globe::Globe(const Transform& transform)
	:GameObject(transform)
{
	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(transform);
	mMeshRender->mMatName = "mirror";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = "sphere";
	mMeshRender->mRenderLayer = (int)RenderLayer::OpaqueDynamicReflectors;
	mMeshRender->mReceiveShadow = true;
	mMeshRender->AddMeshRender();
}

Globe::~Globe()
{
}

void Globe::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
