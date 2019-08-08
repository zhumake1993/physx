#pragma once

#include "Common/d3dUtil.h"
#include "Manager/Material.h"

class MeshRenderCPT
{
public:

	MeshRenderCPT(const Transform& transform);
	virtual ~MeshRenderCPT();

	DirectX::XMFLOAT4X4 GetWorld();

	void AddMeshRender();
	void Update();
	void Release();

private:
	
	//

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