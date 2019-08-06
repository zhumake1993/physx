#pragma once

#include "Common/d3dUtil.h"
#include "Common/FrameResource.h"
#include "MeshManager.h"

struct MeshRenderData
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InverseTransposeWorld = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
	UINT MaterialIndex;
	UINT ReceiveShadow;
	UINT InstancePad1;
	UINT InstancePad2;
};

class MeshRenderInstance
{
	friend class MeshRenderInstanceManager;

public:
	MeshRenderInstance();
	virtual ~MeshRenderInstance();

	std::shared_ptr<Mesh> GetMesh();

	void CalculateBoundingBox();

	void AddMeshRender(const std::string& name, const DirectX::XMFLOAT4X4& world,
		const UINT& matIndex, const DirectX::XMFLOAT4X4& texTransform,
		const bool receiveShadow);

	void DeleteMeshRender(const std::string& name);

	void UpdateMeshRender(const std::string& name, const DirectX::XMFLOAT4X4& world,
		const UINT& matIndex, const DirectX::XMFLOAT4X4& texTransform,
		const bool receiveShadow);

	void UploadMeshRender();

	void Draw();

	bool Pick(DirectX::FXMVECTOR rayOriginW, DirectX::FXMVECTOR rayDirW, std::string& name, float& tmin, DirectX::XMVECTOR& point);

private:
	//

public:
	//

private:

	std::string mMeshName;

	std::shared_ptr<Mesh> mMesh = nullptr;

	UINT mCapacity = 1024;
	UINT mCount = 0;
	std::unique_ptr<FrameResource<MeshRenderData>> mFrameResource; // Ö¡×ÊÔ´

	UINT mVisibleCount = 0;

	DirectX::BoundingBox mBounds;

	std::unordered_map<std::string, MeshRenderData> mMeshRenders;
};