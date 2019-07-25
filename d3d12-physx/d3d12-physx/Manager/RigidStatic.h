#pragma once

#include "Common/d3dUtil.h"
#include "Manager/MeshRender.h"

class RigidStatic
{
public:

	RigidStatic(const std::string& name, const Transform& parent, const Transform& local);
	virtual ~RigidStatic();

	void AddRigidStatic();

private:
	//

public:
	
	std::string mName = "";
	Transform mParentTransform;
	Transform mLocalTransform;

	DirectX::XMFLOAT4 mScale;
	DirectX::XMFLOAT3 mPxMaterial;
	PxGeometryEnum mPxGeometry;

private:
	std::unique_ptr<MeshRender> mMeshRender = nullptr;
};