#include "Skull.h"

Skull::Skull()
	:GameObject()
{
	mGameObjectName = "skull";
	mScale = XMFLOAT3(0.2f, 0.2f, 0.2f);
	mMatName = "skullMat";
	mMeshName = "skull";
	mRenderLayer = (int)RenderLayer::Opaque;

	// 载入并添加mesh
	{
		std::ifstream fin("Models/skull.txt");

		if (!fin) {
			MessageBox(0, L"Models/skull.txt not found.", 0, 0);
			return;
		}

		UINT vcount = 0;
		UINT tcount = 0;
		std::string ignore;

		fin >> ignore >> vcount;
		fin >> ignore >> tcount;
		fin >> ignore >> ignore >> ignore >> ignore;

		std::vector<Vertex> vertices(vcount);
		for (UINT i = 0; i < vcount; ++i) {
			fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
			fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;

			XMVECTOR P = XMLoadFloat3(&vertices[i].Pos);

			// 将点映射至单位球，并产生球纹理坐标
			XMFLOAT3 spherePos;
			XMStoreFloat3(&spherePos, XMVector3Normalize(P));

			float theta = atan2f(spherePos.z, spherePos.x);

			// Put in [0, 2pi].
			if (theta < 0.0f)
				theta += XM_2PI;

			float phi = acosf(spherePos.y);

			float u = theta / (2.0f * XM_PI);
			float v = phi / XM_PI;

			vertices[i].TexC = { u, v };
		}

		fin >> ignore;
		fin >> ignore;
		fin >> ignore;

		std::vector<std::uint16_t> indices(3 * tcount);
		for (UINT i = 0; i < tcount; ++i) {
			fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
		}

		fin.close();

		gMeshManager->AddMesh("skull", vertices, indices);
	}
}

Skull::~Skull()
{
}

void Skull::Update()
{
	mTranslation = XMFLOAT3(3.0f * cos(gTimer.TotalTime()*0.5f), 2.0f, 3.0f * sin(gTimer.TotalTime()*0.5f));
	mRotation = XMFLOAT3(0.0f, gTimer.TotalTime(), 0.0f);
}
