#include "Cylinder.h"

using namespace DirectX;

Cylinder::Cylinder()
	:GameObject()
{
	// ������Ϣ
	mName = "Cylinder";

	// ���MeshRender
	mHasMeshRender = true;
	mMeshRender.MatName = "bricks";
	XMStoreFloat4x4(&mMeshRender.TexTransform, XMMatrixScaling(1.5f, 2.0f, 1.0f));
	mMeshRender.MeshName = "cylinder";
}

Cylinder::~Cylinder()
{
}

void Cylinder::Update()
{
	GameObject::Update();
}
