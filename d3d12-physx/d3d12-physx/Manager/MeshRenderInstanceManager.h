#pragma once

#include "MeshRenderInstance.h"
#include "Component/MeshRenderCPT.h"

class MeshRenderInstanceManager
{
public:
	MeshRenderInstanceManager();
	~MeshRenderInstanceManager();

	void Initialize();

	std::string NewMeshRenderName();
	void AddMeshRender(MeshRenderCPT* meshRenderCPT);
	void DeleteMeshRender(MeshRenderCPT* meshRenderCPT);
	void UpdateMeshRender(MeshRenderCPT* meshRenderCPT);

	void UploadMeshRender();

	void Draw(int randerLayer);

	std::vector<RaycastHit> Raycast(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction);

private:
	//

public:
	std::unordered_map<std::string, std::unique_ptr<MeshRenderInstance>> mMeshRenderInstanceLayers[(int)RenderLayer::Count];

private:
	std::unordered_map<std::string, std::string> mMeshRenderToParent;
};