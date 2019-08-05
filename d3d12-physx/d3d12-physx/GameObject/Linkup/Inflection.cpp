#include "LinkupGameObjects.h"

using namespace DirectX;

Inflection::Inflection(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	// Material
	mMaterial = std::make_shared<Material>();
	mMaterial->mDiffuseMapIndex = -1;
	mMaterial->mNormalMapIndex = -1;
	mMaterial->mDiffuseAlbedo = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mMaterial->mFresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	mMaterial->mRoughness = 0.1f;
	AddMaterial();

	// MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(transform);
	mMeshRenderCPT->mMaterial = mMaterial;
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRenderCPT->mMeshName = "Inflection";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRenderCPT->mReceiveShadow = false;
	mMeshRenderCPT->AddMeshRender();

	Destroy(1.0f);
}

Inflection::~Inflection()
{
}

void Inflection::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
