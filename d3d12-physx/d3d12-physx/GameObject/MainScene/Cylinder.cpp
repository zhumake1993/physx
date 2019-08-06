#include "Cylinder.h"

using namespace DirectX;

Cylinder::Cylinder(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	// Material
	mMaterial = std::make_shared<Material>();
	mMaterial->mDiffuseMapIndex = GetTextureIndex("bricks");
	mMaterial->mNormalMapIndex = GetTextureIndex("bricks_nmap");
	mMaterial->mDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterial->mFresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	mMaterial->mRoughness = 0.3f;
	AddMaterial();

	// MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(transform);
	mMeshRenderCPT->mMaterial = mMaterial;
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.5f, 2.0f, 1.0f));
	mMeshRenderCPT->mMeshName = "cylinder";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRenderCPT->mReceiveShadow = true;
	mMeshRenderCPT->mParent = mName;
	mMeshRenderCPT->AddMeshRender();
}

Cylinder::~Cylinder()
{
}

void Cylinder::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
