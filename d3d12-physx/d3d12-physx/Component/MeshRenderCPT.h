#pragma once

#include "Common/d3dUtil.h"
#include "Manager/Material.h"

struct MeshRender
{
	std::string Name = "";
	std::string Parent = "";
	DirectX::XMFLOAT4X4 World;
	UINT MaterialIndex;
	DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
	std::string MeshName = "";
	int RenderLayer = -1;
	bool ReceiveShadow = true;
};

class MeshRenderCPT
{
public:

	MeshRenderCPT(const Transform& transform);
	virtual ~MeshRenderCPT();

	void AddMeshRender();
	void Update();
	void Release();

private:
	
	DirectX::XMFLOAT4X4 GetWorld();

public:
	
	std::string mName = "";
	Transform mTransform;

	std::shared_ptr<Material> mMaterial;
	DirectX::XMFLOAT4X4 mTexTransform = MathHelper::Identity4x4();
	std::string mMeshName = "";
	int mRenderLayer = -1;
	bool mReceiveShadow = true;

	std::string mParent = "";

private:
	//
};