#include "Box.h"

using namespace DirectX;

Box::Box(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	// Material
	mMaterial = std::make_shared<Material>();
	mMaterial->mDiffuseMapIndex = GetTextureIndex("bricks2");
	mMaterial->mNormalMapIndex = GetTextureIndex("bricks2_nmap");
	mMaterial->mDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterial->mFresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	mMaterial->mRoughness = 0.3f;
	AddMaterial();

	// MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(transform);
	mMeshRenderCPT->mMaterial = mMaterial;
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 0.5f, 1.0f));
	mMeshRenderCPT->mMeshName = "box";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRenderCPT->mReceiveShadow = true;
	mMeshRenderCPT->AddMeshRender();
}

Box::~Box()
{
}

void Box::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
