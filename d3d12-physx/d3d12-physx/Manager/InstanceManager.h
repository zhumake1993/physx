#pragma once

#include "Instance.h"
#include "Manager/MeshRender.h"

class InstanceManager
{
public:
	InstanceManager();
	~InstanceManager();

	void Initialize();

	void AddInstance(MeshRender* meshRender);
	void DeleteInstance(MeshRender* meshRender);
	void UpdateInstance(MeshRender* meshRender);

	void UploadInstanceData();

	void Draw(int randerLayer);

	bool Pick(DirectX::FXMVECTOR rayOriginW, DirectX::FXMVECTOR rayDirW);

private:
	//

public:
	std::unordered_map<std::string, std::unique_ptr<Instance>> mInstanceLayers[(int)RenderLayer::Count];

private:
	std::unordered_map<std::string, MeshRender*> mMeshRenders;
};