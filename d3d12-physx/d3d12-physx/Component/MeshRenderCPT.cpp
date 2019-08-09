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

XMFLOAT4X4 MeshRenderCPT::GetWorld()
{
	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, TransformToMatrix(mTransform));
	return world;
}

void MeshRenderCPT::AddMeshRender()
{
	mName = GetCurrMeshRenderInstanceManager()->NewMeshRenderName();

	GetCurrMeshRenderInstanceManager()->AddMeshRender(this);
}

void MeshRenderCPT::Update(const Transform& transform)
{
	mTransform.Translation = transform.Translation;
	mTransform.Quaternion = transform.Quaternion;

	GetCurrMeshRenderInstanceManager()->UpdateMeshRender(this);
}

void MeshRenderCPT::Release()
{
	GetCurrMeshRenderInstanceManager()->DeleteMeshRender(this);
}