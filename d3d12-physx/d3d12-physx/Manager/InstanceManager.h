#pragma once

#include "Instance.h"

class InstanceManager
{
public:
	InstanceManager();
	~InstanceManager();

	void Initialize();

	void AddInstance(const MeshRender& meshRender);
	void UpdateInstance(const MeshRender& meshRender);

	void UploadInstanceData();

	void Draw(int randerLayer);

	bool Pick(DirectX::FXMVECTOR rayOriginW, DirectX::FXMVECTOR rayDirW);

private:
	//

public:
	std::unordered_map<std::string, std::unique_ptr<Instance>> mInstanceLayers[(int)RenderLayer::Count];

private:
	//
};