#pragma once

#include "Common/d3dUtil.h"
#include "Component/MeshRenderCPT.h"

class RigidStaticCPT
{
public:

	RigidStaticCPT(const std::string& parentName, const Transform& parent, const Transform& local);
	virtual ~RigidStaticCPT();

	void AddRigidStatic();

	void Release();

private:
	//

public:
	
	std::string mName = "";
	std::string mParentName = "";
	Transform mParentTransform;
	Transform mLocalTransform;

	DirectX::XMFLOAT4 mScale;
	DirectX::XMFLOAT3 mPxMaterial;
	PxGeometryEnum mPxGeometry;

private:
	std::shared_ptr<MeshRenderCPT> mMeshRenderCPT = nullptr;
	Transform mWorldTransform; // 缓存刚体的世界坐标
};