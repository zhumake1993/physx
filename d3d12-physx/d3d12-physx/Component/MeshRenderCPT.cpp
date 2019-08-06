#include "MeshRenderCPT.h"
#include "Manager/SceneManager.h"

using namespace DirectX;

extern XMMATRIX TransformToMatrix(Transform& transform);

MeshRenderCPT::MeshRenderCPT(const Transform& transform)
{
	mTransform = transform;
}

MeshRenderCPT::~MeshRenderCPT()
{
}

void MeshRenderCPT::AddMeshRender()
{
	mName = GetCurrMeshRenderInstanceManager()->NewMeshRenderName();

	auto meshRender = std::make_shared<MeshRender>();
	meshRender->Name = mName;
	meshRender->Parent = mParent;
	meshRender->World = GetWorld();
	meshRender->MaterialIndex = mMaterial->mIndex;
	meshRender->TexTransform = mTexTransform;
	meshRender->MeshName = mMeshName;
	meshRender->RenderLayer = mRenderLayer;
	meshRender->ReceiveShadow = mReceiveShadow;

	GetCurrMeshRenderInstanceManager()->AddMeshRender(meshRender);
}

void MeshRenderCPT::Update()
{
	auto meshRender = std::make_shared<MeshRender>();
	meshRender->Name = mName;
	meshRender->Parent = mParent;
	meshRender->World = GetWorld();
	meshRender->MaterialIndex = mMaterial->mIndex;
	meshRender->TexTransform = mTexTransform;
	meshRender->MeshName = mMeshName;
	meshRender->RenderLayer = mRenderLayer;
	meshRender->ReceiveShadow = mReceiveShadow;

	GetCurrMeshRenderInstanceManager()->UpdateMeshRender(meshRender);
}

void MeshRenderCPT::Release()
{
	auto meshRender = std::make_shared<MeshRender>();
	meshRender->Name = mName;
	meshRender->Parent = mParent;
	meshRender->World = GetWorld();
	meshRender->MaterialIndex = mMaterial->mIndex;
	meshRender->TexTransform = mTexTransform;
	meshRender->MeshName = mMeshName;
	meshRender->RenderLayer = mRenderLayer;
	meshRender->ReceiveShadow = mReceiveShadow;

	GetCurrMeshRenderInstanceManager()->DeleteMeshRender(meshRender);
}

XMFLOAT4X4 MeshRenderCPT::GetWorld()
{
	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, TransformToMatrix(mTransform));
	return world;
}