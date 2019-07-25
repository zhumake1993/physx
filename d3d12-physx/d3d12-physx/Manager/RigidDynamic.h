#pragma once

#include "Common/d3dUtil.h"
#include "Manager/MeshRender.h"

class RigidDynamic
{
public:

	RigidDynamic(const std::string& name, const Transform& parent, const Transform& local);
	virtual ~RigidDynamic();

	void AddRigidDynamic();

	void Update();

private:
	//

public:
	
	std::string mName = "";
	Transform mParentTransform;
	Transform mLocalTransform;

	DirectX::XMFLOAT4 mScale;
	DirectX::XMFLOAT3 mPxMaterial;
	PxGeometryEnum mPxGeometry;
	float mDensity;

private:
	Transform mLocalTransformReverse;
	std::unique_ptr<MeshRender> mMeshRender = nullptr;
};