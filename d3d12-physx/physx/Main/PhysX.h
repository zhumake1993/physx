#pragma once

#include <string>

#include "../physx/Common/PhysXCommon.h"

class PhysX
{
public:
	PhysX();
	~PhysX();

	void InitPhysics();

	// 场景
	void CreateScene();
	void CleanupScene();

	// 静态刚体
	void CreatePxRigidStatic(std::string name, void* pdesc);

	// 动态刚体
	void CreatePxRigidDynamic(std::string name, void* pdesc);
	void SetAngularDamping(std::string name, float ad);
	void SetLinearVelocity(std::string name, PxFloat3 v);

	// 刚体通用方法
	void DeletePxRigid(std::string name);

	void Update(float delta);

	void GetPxRigidDynamicTransform(std::string name, PxFloat3& pos, PxFloat4& quat);

	void CleanupPhysics();

private:
	bool HasPxRigidStatic(const std::string& name);
	bool HasPxRigidDynamic(const std::string& name);

public:
	//

private:
	
	float mAccumulator = 0.0f;
	float mStepSize = 1.0f / 60.0f;
};
