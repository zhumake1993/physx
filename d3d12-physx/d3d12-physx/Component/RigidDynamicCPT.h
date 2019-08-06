#pragma once

#include "Common/d3dUtil.h"
#include "Component/MeshRenderCPT.h"

class RigidDynamicCPT
{
public:

	RigidDynamicCPT(const Transform& parent, const Transform& local);
	virtual ~RigidDynamicCPT();

	void AddRigidDynamic();

	void Update();

	void Release();

	void SetAngularDamping(float ad);
	void SetLinearVelocity(DirectX::XMFLOAT3 v);

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
	std::shared_ptr<MeshRenderCPT> mMeshRenderCPT = nullptr;
};