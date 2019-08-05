#include "LinkupGameObjects.h"

using namespace DirectX;

Floor::Floor(const Transform& transform, const std::string& name)
	:GameObject(transform, name)
{
	// Material
	mMaterial = std::make_shared<Material>();
	mMaterial->mDiffuseMapIndex = GetTextureIndex("tile");
	mMaterial->mNormalMapIndex = GetTextureIndex("tile_nmap");
	mMaterial->mDiffuseAlbedo = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	mMaterial->mFresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	mMaterial->mRoughness = 0.1f;
	AddMaterial();

	// MeshRender
	mMeshRenderCPT = std::make_shared<MeshRenderCPT>(transform);
	mMeshRenderCPT->mMaterial = mMaterial;
	XMStoreFloat4x4(&mMeshRenderCPT->mTexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
	mMeshRenderCPT->mMeshName = "grid";
	mMeshRenderCPT->mRenderLayer = (int)RenderLayer::Opaque;
	mMeshRenderCPT->mReceiveShadow = true;
	mMeshRenderCPT->AddMeshRender();

	// Rigidbody
	Transform rigidStaticLocal = Transform(XMFLOAT3(0.0f, -0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	mRigidStaticCPT = std::make_shared<RigidStaticCPT>(transform, rigidStaticLocal);
	mRigidStaticCPT->mScale = XMFLOAT4(10.0f, 0.5f, 15.0f, 0.5f);
	mRigidStaticCPT->mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.6f);
	mRigidStaticCPT->mPxGeometry = PxBoxEnum;
	mRigidStaticCPT->AddRigidStatic();
}

Floor::~Floor()
{
}

void Floor::Update(const GameTimer& gt)
{
	GameObject::Update(gt);
}
