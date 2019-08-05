#include "Sky.h"

using namespace DirectX;

Sky::Sky(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	// Material
	mMaterial = std::make_shared<Material>();
	mMaterial->mDiffuseMapIndex = 0;
	mMaterial->mNormalMapIndex = -1;
	mMaterial->mDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterial->mFresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
	mMaterial->mRoughness = 0.1f;
	AddMaterial();

	// MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(transform);
	mMeshRenderCPT->mMaterial = mMaterial;
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRenderCPT->mMeshName = "sphere";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Sky;
	mMeshRenderCPT->mReceiveShadow = false;
	mMeshRenderCPT->AddMeshRender();
}

Sky::~Sky()
{
}

void Sky::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
