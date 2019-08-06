#include "Globe.h"

using namespace DirectX;

Globe::Globe(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	// Material
	mMaterial = std::make_shared<Material>();
	mMaterial->mDiffuseMapIndex = GetTextureIndex("white1x1");
	mMaterial->mNormalMapIndex = GetTextureIndex("default_nmap");
	mMaterial->mDiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mMaterial->mFresnelR0 = XMFLOAT3(0.98f, 0.97f, 0.95f);
	mMaterial->mRoughness = 0.1f;
	AddMaterial();

	// MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(transform);
	mMeshRenderCPT->mMaterial = mMaterial;
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRenderCPT->mMeshName = "sphere";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::OpaqueDynamicReflectors;
	mMeshRenderCPT->mReceiveShadow = true;
	mMeshRenderCPT->mParent = mName;
	mMeshRenderCPT->AddMeshRender();
}

Globe::~Globe()
{
}

void Globe::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
