#include "Sky.h"

using namespace DirectX;

Sky::Sky()
	:GameObject()
{
	// ������Ϣ
	mName = "sky";
	mTransform.Scale = XMFLOAT3(5000.0f, 5000.0f, 5000.0f);

	// ���MeshRender
	mHasMeshRender = true;
	mMeshRender.MatName = "sky";
	mMeshRender.MeshName = "sphere";
	mMeshRender.RenderLayer = (int)RenderLayer::Sky;
	mMeshRender.ReceiveShadow = false;
}

Sky::~Sky()
{
}

void Sky::Update()
{
	GameObject::Update();
}
