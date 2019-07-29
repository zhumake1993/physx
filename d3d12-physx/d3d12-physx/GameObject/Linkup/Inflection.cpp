#include "LinkupGameObjects.h"

using namespace DirectX;

Inflection::Inflection(const std::string& name, const Transform& transform)
	:GameObject(name, transform)
{
	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(name, transform);
	mMeshRender->mMatName = "Line";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = "Inflection";
	mMeshRender->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRender->mReceiveShadow = false;
	mMeshRender->AddMeshRender();
}

Inflection::~Inflection()
{
}

void Inflection::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
