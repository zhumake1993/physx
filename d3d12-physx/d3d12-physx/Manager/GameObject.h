#pragma once

#include "Common/d3dUtil.h"
#include "Common/PhysXUtil.h"

class GameObject
{
	friend class GameObjectManager;
public:
	GameObject();
	virtual ~GameObject();

	virtual void Update();

protected:
	bool GetKeyDown(int key);
	bool GetKeyPress(int key);
	bool GetKeyUp(int key);

	void AddMeshRender();
	void AddRigidBody();

public:
	std::string mName = "";
	Transform mTransform;

	bool mHasMeshRender = false;
	MeshRender mMeshRender;

	bool mHasRigidBody = false;
	Transform mRigidBodyLocalTransform;
	DirectX::XMFLOAT3 mPxMaterial;
	PxGeometryEnum mPxGeometry;
	float mDensity;
	MeshRender mRigidBodyMeshRender;
};