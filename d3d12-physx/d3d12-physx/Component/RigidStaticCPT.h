#pragma once

#include "Common/d3dUtil.h"
#include "Component/MeshRenderCPT.h"

class RigidStaticCPT
{
public:

	RigidStaticCPT(const Transform& parent, const Transform& local);
	virtual ~RigidStaticCPT();

	void AddRigidStatic();

	void Release();

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
	std::shared_ptr<MeshRenderCPT> mMeshRenderCPT = nullptr;
};