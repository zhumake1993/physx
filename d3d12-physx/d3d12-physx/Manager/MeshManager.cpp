#include "MeshManager.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

extern ComPtr<ID3D12Device> gD3D12Device;
extern ComPtr<ID3D12GraphicsCommandList> gCommandList;

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
	for (const auto& p : mMeshes) {
		p.second->VertexBufferUploader = nullptr;
		p.second->IndexBufferUploader = nullptr;
	}
}

void MeshManager::Initialize()
{
	// skull
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

			DirectX::XMVECTOR P = XMLoadFloat3(&vertices[i].Pos);

			// 将点映射至单位球，并产生球纹理坐标
			DirectX::XMFLOAT3 spherePos;
			DirectX::XMStoreFloat3(&spherePos, DirectX::XMVector3Normalize(P));

			float theta = atan2f(spherePos.z, spherePos.x);

			// Put in [0, 2pi].
			if (theta < 0.0f)
				theta += DirectX::XM_2PI;

			float phi = acosf(spherePos.y);

			float u = theta / (2.0f * DirectX::XM_PI);
			float v = phi / DirectX::XM_PI;

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

		AddMesh("skull", vertices, indices);
	}
}

bool MeshManager::HasMesh(std::string name)
{
	return mMeshes.find(name) != mMeshes.end();
}

std::shared_ptr<Mesh> MeshManager::GetMesh(const std::string name)
{
	if (!HasMesh(name)) {
		ThrowMyEx("Mesh does not exist!")
	}

	return mMeshes[name];
}

void MeshManager::AddMesh(std::string Name, GeometryGenerator::MeshData mesh)
{
	if (HasMesh(Name)) {
		ThrowMyEx("Mesh already exists!")
	}

	// 注意：Vertex与GeometryGenerator::Vertex是不一样的
	// 必须将mesh中的顶点向量和索引向量重新复制一份
	std::vector<Vertex> vertices(mesh.Vertices.size());
	for (size_t i = 0; i < mesh.Vertices.size(); ++i) {
		vertices[i].Pos = mesh.Vertices[i].Position;
		vertices[i].Normal = mesh.Vertices[i].Normal;
		vertices[i].TexC = mesh.Vertices[i].TexC;
		vertices[i].TangentU = mesh.Vertices[i].TangentU;
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(mesh.GetIndices16()), std::end(mesh.GetIndices16()));

	AddMesh(Name, vertices, indices);
}

void MeshManager::AddMesh(std::string Name, std::vector<Vertex>& vertices, std::vector<std::uint16_t>& indices)
{
	if (HasMesh(Name)) {
		ThrowMyEx("Mesh already exists!")
	}

	auto meshGeo = std::make_shared<Mesh>();
	meshGeo->Name = Name;

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &meshGeo->VertexBufferCPU));
	CopyMemory(meshGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &meshGeo->IndexBufferCPU));
	CopyMemory(meshGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	meshGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(gD3D12Device.Get(),
		gCommandList.Get(), vertices.data(), vbByteSize, meshGeo->VertexBufferUploader);

	meshGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(gD3D12Device.Get(),
		gCommandList.Get(), indices.data(), ibByteSize, meshGeo->IndexBufferUploader);

	meshGeo->VertexByteStride = sizeof(Vertex);
	meshGeo->VertexBufferByteSize = vbByteSize;
	meshGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	meshGeo->IndexBufferByteSize = ibByteSize;
	meshGeo->IndexCount = (UINT)indices.size();

	meshGeo->VertexBufferView.BufferLocation = meshGeo->VertexBufferGPU->GetGPUVirtualAddress();
	meshGeo->VertexBufferView.StrideInBytes = meshGeo->VertexByteStride;
	meshGeo->VertexBufferView.SizeInBytes = meshGeo->VertexBufferByteSize;

	meshGeo->IndexBufferView.BufferLocation = meshGeo->IndexBufferGPU->GetGPUVirtualAddress();
	meshGeo->IndexBufferView.Format = meshGeo->IndexFormat;
	meshGeo->IndexBufferView.SizeInBytes = meshGeo->IndexBufferByteSize;

	mMeshes[Name] = meshGeo;
}