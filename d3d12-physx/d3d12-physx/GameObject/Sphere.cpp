#include "Sphere.h"

using namespace DirectX;

Sphere::Sphere()
	:GameObject()
{
	// ������Ϣ
	mName = "Sphere";

	// ���MeshRender
	mHasMeshRender = true;
	mMeshRender.MatName = "mirror";
	mMeshRender.MeshName = "sphere";
}

Sphere::~Sphere()
{
}

void Sphere::Update()
{
	GameObject::Update();
}
