#pragma once

#include "Common/d3dUtil.h"
#include "Common/FrameResource.h"

struct MaterialData
{
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };

	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.5f;

	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();

	DirectX::XMFLOAT4 LerpDiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };

	UINT DiffuseMapIndex = 0;
	UINT NormalMapIndex = 0;
	float LerpPara = 0;
	UINT MaterialPad2;
};

class MaterialManager
{
public:

	MaterialManager();
	~MaterialManager();

	void Initialize();

	bool HasMaterial(std::string name);
	std::shared_ptr<MaterialData> GetMaterial(const std::string name);
	UINT GetIndex(const std::string& name);
	void AddMaterial(const std::string& name, std::shared_ptr<MaterialData> materialData);
	std::string AddMaterial(std::shared_ptr<MaterialData> materialData);

	void UpdateMaterialData();

	ID3D12Resource* CurrResource()const;

private:
	//

public:
	//

private:

	const UINT mMaterialDataCapacity = 100;
	UINT mMaterialCount = 0;

	std::unique_ptr<FrameResource<MaterialData>> mFrameResource; // 帧资源

	std::unordered_map<std::string, std::shared_ptr<MaterialData>> mMaterials;
	std::unordered_map<std::string, UINT> mIndices;
	std::unordered_map<std::string, int> mNumFramesDirties; // 指示对象数据发生变化
};