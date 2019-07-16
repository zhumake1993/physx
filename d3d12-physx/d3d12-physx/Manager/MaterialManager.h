#pragma once

#include "Common/d3dUtil.h"
#include "Common/UploadBuffer.h"

using namespace DirectX;

struct MaterialData
{
	XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.5f;

	XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();

	UINT DiffuseMapIndex = 0;
	UINT NormalMapIndex = 0;
	UINT MaterialPad1;
	UINT MaterialPad2;
};

class MaterialManager
{
public:

	MaterialManager();
	~MaterialManager();

	void Initialize();

	UINT GetIndex(const std::string& name);

	void AddMaterial(const std::string& name, UINT diffuseIndex, UINT normalIndex,
		const XMFLOAT4& diffuseAlbedo, const XMFLOAT3& fresnelR0, float roughness,
		const XMFLOAT4X4& matTransform);
	void AddMaterial(const std::string& name, const MaterialData& mat);
	void DeleteMaterial(std::string name);

	MaterialData GetMaterialData(const std::string& name);
	void SetMaterialData(const std::string& name, MaterialData& mat);

	void UpdateMaterialData();

	ID3D12Resource* CurrResource()const;

private:
	//

public:
	//

private:

	UINT mMaterialDataCapacity = 100;
	UINT mMaterialCount = 1;
	std::vector<std::unique_ptr<UploadBuffer<MaterialData>>> mFrameResources; // 帧资源vector

	std::unordered_map<std::string, MaterialData> mMaterials;
	std::unordered_map<std::string, UINT> mIndices;
	std::unordered_map<std::string, int> mNumFramesDirties; // 指示对象数据发生变化
};

extern std::unique_ptr<MaterialManager> gMaterialManager;