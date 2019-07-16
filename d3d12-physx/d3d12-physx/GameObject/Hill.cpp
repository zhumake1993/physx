#include "Hill.h"

Hill::Hill()
	:GameObject()
{
	mGameObjectName = "hill";
	mTranslation = XMFLOAT3(0.0f, -20.0f, 0.0f);
	mMatName = "grass";
	XMStoreFloat4x4(&mTexTransform, XMMatrixScaling(5.0f, 5.0f, 1.0f));
	mMeshName = "hill";
	mRenderLayer = (int)RenderLayer::Opaque;

	GeometryGenerator geoGen;
	auto hill = geoGen.CreateGrid(160.0f, 160.0f, 50, 50);
	for (auto& v : hill.Vertices) {
		v.Position.y = 0.3f * (v.Position.z * sinf(0.1f * v.Position.x) + v.Position.x * cosf(0.1f * v.Position.z));

		XMFLOAT3 n(
			-0.03f * v.Position.z * cosf(0.1f * v.Position.x) - 0.3f * cosf(0.1f * v.Position.z),
			1.0f,
			-0.3f * sinf(0.1f * v.Position.x) + 0.03f * v.Position.x * sinf(0.1f * v.Position.z));

		XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
		XMStoreFloat3(&n, unitNormal);
		v.Normal = n;
	}
	gMeshManager->AddMesh("hill", hill);

	mReceiveShadow = false;
}

Hill::~Hill()
{
}

void Hill::Update()
{
}
