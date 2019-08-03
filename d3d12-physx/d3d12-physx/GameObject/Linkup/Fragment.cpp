#include "LinkupGameObjects.h"

using namespace DirectX;

Fragment::Fragment(const Transform& transform)
	:GameObject(transform)
{
	// 个人材质
	auto red = std::make_shared<MaterialData>();
	red->DiffuseMapIndex = -1;
	red->NormalMapIndex = -1;
	red->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	red->FresnelR0 = XMFLOAT3(0.2f, 0.2f, 0.2f);
	red->Roughness = 0.1f;
	red->LerpDiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	red->LerpPara = 0.0f;
	auto matName = AddMaterial(red);

	// MeshRender
	mMeshRender = std::make_unique<MeshRender>(transform);
	mMeshRender->mMatName = matName;
	XMStoreFloat4x4(&mMeshRender->mTexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	mMeshRender->mMeshName = "box";
	mMeshRender->mRenderLayer = (int)RenderLayer::Transparent;
	mMeshRender->mReceiveShadow = true;
	mMeshRender->AddMeshRender();

	// 刚体
	Transform rigidDynamicLocal = Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	mRigidDynamic = std::make_unique<RigidDynamic>(transform, rigidDynamicLocal);
	mRigidDynamic->mScale = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.1f);
	mRigidDynamic->mPxMaterial = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mRigidDynamic->mPxGeometry = PxBoxEnum;
	mRigidDynamic->mDensity = 10.0f;
	mRigidDynamic->AddRigidDynamic();

	Destroy(2.0f);
}

Fragment::~Fragment()
{
}

void Fragment::SetColor(DirectX::XMFLOAT3 color)
{
	auto mat = GetMaterial();
	mat->DiffuseAlbedo = XMFLOAT4(color.x, color.y, color.z, 1.0f);
	mat->LerpDiffuseAlbedo = XMFLOAT4(color.x, color.y, color.z, 0.0f);
}

void Fragment::Update(const GameTimer& gt)
{
	GameObject::Update(gt);

	auto mat = GetMaterial();
	mat->LerpPara = mGameTimer->TotalTime() / mLifeTime;
}
