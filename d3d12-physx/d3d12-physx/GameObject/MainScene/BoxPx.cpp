#include "BoxPx.h"

using namespace DirectX;

BoxPx::BoxPx(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	mIsStatic = false;

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
	mMeshRenderCPT->mParent = mName;
	mMeshRenderCPT->AddMeshRender();

	// ∏’ÃÂ
	Transform rigidDynamicLocal = Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	mRigidDynamicCPT = std::make_shared<RigidDynamicCPT>(transform, rigidDynamicLocal);
	mRigidDynamicCPT->mScale = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	mRigidDynamicCPT->mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mRigidDynamicCPT->mPxGeometry = PxBoxEnum;
	mRigidDynamicCPT->mDensity = 10.0f;
	mRigidDynamicCPT->isKinematic = false;
	mRigidDynamicCPT->AddRigidDynamic();
}

BoxPx::~BoxPx()
{
}

void BoxPx::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
