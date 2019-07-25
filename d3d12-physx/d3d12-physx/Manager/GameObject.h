#pragma once

#include "Common/d3dUtil.h"
#include "Manager/MeshRender.h"
#include "Manager/RigidDynamic.h"
#include "Manager/RigidStatic.h"

class GameObject
{
	friend class GameObjectManager;
public:
	GameObject(const std::string& name, const Transform& transform);
	virtual ~GameObject();

	virtual void Update();

protected:
	bool GetKeyDown(int key);
	bool GetKeyPress(int key);
	bool GetKeyUp(int key);

public:
	std::string mName;
	Transform mTransform;

	std::unique_ptr<MeshRender> mMeshRender = nullptr;
	std::unique_ptr<RigidDynamic> mRigidDynamic = nullptr;
	std::unique_ptr<RigidStatic> mRigidStatic = nullptr;
};