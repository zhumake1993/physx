#include "Skull.h"

using namespace DirectX;

Skull::Skull(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	// Material
	mMaterial = std::make_shared<Material>();
	mMaterial->mDiffuseMapIndex = GetTextureIndex("white1x1");
	mMaterial->mNormalMapIndex = -1;
	mMaterial->mDiffuseAlbedo = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mMaterial->mFresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	mMaterial->mRoughness = 0.2f;
	AddMaterial();

	// MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(transform);
	mMeshRenderCPT->mMaterial = mMaterial;
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRenderCPT->mMeshName = "skull";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRenderCPT->mReceiveShadow = true;
	mMeshRenderCPT->AddMeshRender();
}

Skull::~Skull()
{
}

void Skull::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	mTransform.Translation = XMFLOAT3(3.0f * cos(gt.TotalTime()*0.5f), 2.0f, 3.0f * sin(gt.TotalTime()*0.5f));
	XMStoreFloat4(&mTransform.Quaternion, XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), gt.TotalTime()));

	if (GetKeyDown('X')) {
		SwitchScene("Linkup");
	}
}
