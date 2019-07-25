#pragma once

#include "Common/d3dUtil.h"

class MeshRender
{
public:
	MeshRender(const std::string& name, const Transform& transform);
	virtual ~MeshRender();

	DirectX::XMFLOAT4X4 GetWorld();

	void AddMeshRender();

	void Update();

private:
	//

public:
	
	std::string mName = "";
	Transform mTransform;

	std::string mMatName = "";
	DirectX::XMFLOAT4X4 mTexTransform = MathHelper::Identity4x4();
	std::string mMeshName = "";
	int mRenderLayer = -1;
	bool mReceiveShadow = true;
};