#include "LinkupGameObjects.h"

using namespace DirectX;

Fragment::Fragment(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	// Material
	mMaterial = std::make_shared<Material>();
	mMaterial->mDiffuseMapIndex = -1;
	mMaterial->mNormalMapIndex = -1;
	mMaterial->mDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterial->mFresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	mMaterial->mRoughness = 0.1f;
	mMaterial->mLerpDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterial->mLerpPara = 0.0f;
	AddMaterial();

	// MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(transform);
	mMeshRenderCPT->mMaterial = mMaterial;
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRenderCPT->mMeshName = "box";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Transparent;
	mMeshRenderCPT->mReceiveShadow = true;
	mMeshRenderCPT->mParent = mName;
	mMeshRenderCPT->AddMeshRender();

	// ∏’ÃÂ
	Transform rigidDynamicLocal = Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	mRigidDynamicCPT = std::make_shared<RigidDynamicCPT>(transform, rigidDynamicLocal);
	mRigidDynamicCPT->mScale = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f);
	mRigidDynamicCPT->mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mRigidDynamicCPT->mPxGeometry = PxBoxEnum;
	mRigidDynamicCPT->mDensity = 10.0f;
	mRigidDynamicCPT->AddRigidDynamic();

	Destroy(2.0f);
}

Fragment::~Fragment()
{
}

void Fragment::SetColor(DirectX::XMFLOAT3 color)
{
	auto mat = GetMaterial();
	mat->mDiffuseAlbedo = XMFLOAT4(color.x, color.y, color.z, 1.0f);
	mat->mLerpDiffuseAlbedo = XMFLOAT4(color.x, color.y, color.z, 0.0f);
	SetMaterial(mat);
}

void Fragment::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	auto mat = GetMaterial();
	mat->mLerpPara = mGameTimer->TotalTime() / mLifeTime;
	SetMaterial(mat);
}

void Fragment::Release()
{
	GameObject::Release();
}