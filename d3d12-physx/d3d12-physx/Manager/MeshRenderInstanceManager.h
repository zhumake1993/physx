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
	void AddMeshRender(std::shared_ptr<MeshRender> meshRender);
	void DeleteMeshRender(std::shared_ptr<MeshRender> meshRender);
	void UpdateMeshRender(std::shared_ptr<MeshRender> meshRender);

	void UploadMeshRender();

	void Draw(int randerLayer);

	bool Pick(DirectX::FXMVECTOR rayOriginW, DirectX::FXMVECTOR rayDirW);

private:
	//

public:
	std::unordered_map<std::string, std::unique_ptr<MeshRenderInstance>> mMeshRenderInstanceLayers[(int)RenderLayer::Count];

private:
	std::unordered_map<std::string, std::string> mMeshRenderToParent;
};