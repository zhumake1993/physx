#pragma once

#include "Instance.h"
#include "MaterialManager.h"

using namespace DirectX;

class InstanceManager
{
public:
	InstanceManager();
	~InstanceManager();

	void Initialize();

	void AddInstance(const std::string& gameObjectName, const XMFLOAT4X4& world,
		const std::string& matName, const XMFLOAT4X4& texTransform,
		const std::string& meshName, const int randerLayer,
		const bool receiveShadow);

	void UpdateInstance(const std::string& gameObjectName, const XMFLOAT4X4& world,
		const std::string& matName, const XMFLOAT4X4& texTransform,
		const std::string& meshName, const int randerLayer,
		const bool receiveShadow);

	void UploadInstanceData();

	void Draw(int randerLayer);

	bool Pick(FXMVECTOR rayOriginW, FXMVECTOR rayDirW);

private:
	//

public:
	std::unordered_map<std::string, std::unique_ptr<Instance>> mInstanceLayers[(int)RenderLayer::Count];

private:
	//
};

extern std::unique_ptr<InstanceManager> gInstanceManager;