#include "Cylinder.h"

using namespace DirectX;

Cylinder::Cylinder(const std::string& name, const Transform& transform)
	:GameObject(name, transform)
{
	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(name, transform);
	mMeshRender->mMatName = "bricks";
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.5f, 2.0f, 1.0f));
	mMeshRender->mMeshName = "cylinder";
	mMeshRender->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRender->mReceiveShadow = true;
	mMeshRender->AddMeshRender();
}

Cylinder::~Cylinder()
{
}

void Cylinder::Update()
{
	GameObject::Update();
}
