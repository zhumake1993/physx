#include "LinkupGameObjects.h"

using namespace DirectX;

Segment::Segment(const std::string& name, const Transform& transform)
	:GameObject(name, transform)
{
	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(name, transform);
	mMeshRender->mMatName = "Line";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = "Segment";
	mMeshRender->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRender->mReceiveShadow = false;
	mMeshRender->AddMeshRender();
}

Segment::~Segment()
{
}

void Segment::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
