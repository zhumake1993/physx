#include "MeshRender.h"

using namespace DirectX;

#include "Manager/SceneManager.h"
extern std::unique_ptr<SceneManager> gSceneManager;

extern XMMATRIX TransformToMatrix(Transform& transform);

MeshRender::MeshRender(const Transform& transform)
{
	mTransform = transform;
}

MeshRender::~MeshRender()
{
}

XMFLOAT4X4 MeshRender::GetWorld()
{
	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, TransformToMatrix(mTransform));
	return world;
}

void MeshRender::AddMeshRender()
{
	gSceneManager->GetCurrInstanceManager()->AddInstance(this);
}

void MeshRender::Update()
{
	gSceneManager->GetCurrInstanceManager()->UpdateInstance(this);
}

void MeshRender::Release()
{
	gSceneManager->GetCurrInstanceManager()->DeleteInstance(this);
}

void MeshRender::SetParent(std::string name)
{
	mParent = name;
}

std::string MeshRender::GetParent()
{
	return mParent;
}
