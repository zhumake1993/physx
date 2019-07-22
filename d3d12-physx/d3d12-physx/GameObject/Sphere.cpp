#include "Sphere.h"

using namespace DirectX;

Sphere::Sphere()
	:GameObject()
{
	// 基础信息
	mName = "Sphere";

	// 添加MeshRender
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
