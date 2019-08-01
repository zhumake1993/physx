#include "LinkupGameObjects.h"

using namespace DirectX;

Inflection::Inflection(const Transform& transform)
	:GameObject(transform)
{
	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(transform);
	mMeshRender->mMatName = "Line";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = "Inflection";
	mMeshRender->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRender->mReceiveShadow = false;
	mMeshRender->AddMeshRender();

	Destroy(1.0f);
}

Inflection::~Inflection()
{
}

void Inflection::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
