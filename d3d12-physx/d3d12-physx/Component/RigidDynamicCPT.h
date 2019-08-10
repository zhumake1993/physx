#pragma once

#include "Common/d3dUtil.h"
#include "Component/MeshRenderCPT.h"

class RigidDynamicCPT
{
public:

	RigidDynamicCPT(const std::string& parentName, const Transform& parent, const Transform& local);
	virtual ~RigidDynamicCPT();

	void AddRigidDynamic();

	void Update(Transform& parentTransform);

	void Release();

	void AddForce(DirectX::XMFLOAT3 force);
	void SetRigidDynamicLockFlag(int axis, bool st);
	void SetAngularDamping(float ad);
	void SetLinearVelocity(DirectX::XMFLOAT3 v);

private:
	
	void PushTransform(const Transform& parentTransform);
	void PullTransform();

public:
	
	std::string mName = "";
	std::string mParentName = "";
	Transform mParentTransform;
	Transform mLocalTransform;

	DirectX::XMFLOAT4 mScale;
	DirectX::XMFLOAT3 mPxMaterial;
	PxGeometryEnum mPxGeometry;
	float mDensity;

	bool isKinematic = false;

private:
	DirectX::XMFLOAT4X4 mLocalMatrixReverse;
	std::shared_ptr<MeshRenderCPT> mMeshRenderCPT = nullptr;
	Transform mWorldTransform; // 缓存刚体的世界坐标
};