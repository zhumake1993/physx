#pragma once

#include "Common/d3dUtil.h"
#include "Component/MeshRenderCPT.h"

class RigidDynamicCPT
{
public:

	RigidDynamicCPT(const Transform& parent, const Transform& local);
	virtual ~RigidDynamicCPT();

	void AddRigidDynamic();

	void Update(Transform& parentTransform);

	void Release();

	void SetAngularDamping(float ad);
	void SetLinearVelocity(DirectX::XMFLOAT3 v);

private:
	
	void PushTransform(const Transform& parentTransform);
	void PullTransform();

public:
	
	std::string mName = "";
	Transform mParentTransform;
	Transform mLocalTransform;

	DirectX::XMFLOAT4 mScale;
	DirectX::XMFLOAT3 mPxMaterial;
	PxGeometryEnum mPxGeometry;
	float mDensity;

	bool isKinematic = false;

private:
	Transform mLocalTransformReverse;
	std::shared_ptr<MeshRenderCPT> mMeshRenderCPT = nullptr;
	Transform mWorldTransform; // 缓存刚体的世界坐标
};