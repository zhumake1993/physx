#pragma once

#include "Common/d3dUtil.h"
#include "Common/FrameResource.h"
#include "MeshManager.h"

struct InstanceData
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InverseTransposeWorld = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
	UINT MaterialIndex;
	UINT ReceiveShadow;
	UINT InstancePad1;
	UINT InstancePad2;
};

class Instance
{
	friend class InstanceManager;

public:
	Instance();
	virtual ~Instance();

	std::shared_ptr<Mesh> GetMesh();

	void CalculateBoundingBox();

	bool HasInstanceData(const std::string& gameObjectName);

	void AddInstanceData(const std::string& gameObjectName, const DirectX::XMFLOAT4X4& world,
		const UINT& matIndex, const DirectX::XMFLOAT4X4& texTransform,
		const bool receiveShadow);

	void DeleteInstanceData(const std::string& gameObjectName);

	void UpdateInstanceData(const std::string& gameObjectName, const DirectX::XMFLOAT4X4& world,
		const UINT& matIndex, const DirectX::XMFLOAT4X4& texTransform,
		const bool receiveShadow);

	void UploadInstanceData();

	void Draw();

	bool Pick(DirectX::FXMVECTOR rayOriginW, DirectX::FXMVECTOR rayDirW, std::string& name, float& tmin, DirectX::XMVECTOR& point);

private:
	//

public:
	//

private:

	std::string mMeshName;

	std::shared_ptr<Mesh> mMesh = nullptr;

	UINT mInstanceDataCapacity = 100;
	UINT mInstanceCount = 0;
	std::unique_ptr<FrameResource<InstanceData>> mFrameResource; // Ö¡×ÊÔ´

	UINT mVisibleCount = 0;

	DirectX::BoundingBox mBounds;

	std::unordered_map<std::string, InstanceData> mInstances;
};