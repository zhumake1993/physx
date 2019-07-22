#include "Globe.h"

using namespace DirectX;

Globe::Globe()
	:GameObject()
{
	// ������Ϣ
	mName = "globe";
	mTransform.Translation = XMFLOAT3(0.0f, 2.0f, 0.0f);
	mTransform.Scale = XMFLOAT3(2.0f, 2.0f, 2.0f);

	// ���MeshRender
	mHasMeshRender = true;
	mMeshRender.MatName = "mirror";
	mMeshRender.MeshName = "sphere";
	mMeshRender.RenderLayer = (int)RenderLayer::OpaqueDynamicReflectors;
}

Globe::~Globe()
{
}

void Globe::Update()
{
	GameObject::Update();
}
