#include "LinkupGameObjects.h"

using namespace DirectX;

Segment::Segment(const Transform& transform)
	:GameObject(transform)
{
	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(transform);
	mMeshRender->mMatName = "Line";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = "Segment";
	mMeshRender->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRender->mReceiveShadow = false;
	mMeshRender->AddMeshRender();

	Destroy(1.0f);
}

Segment::~Segment()
{
}

void Segment::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
