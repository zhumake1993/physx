#pragma once

#include <string>

#include "../d3d12-physx/Common/MathHelper.h"
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
	std::string CreatePxRigidStatic(void* pdesc);

	// 动态刚体
	std::string CreatePxRigidDynamic(void* pdesc);
	void AddForce(std::string name, PxFloat3 v);
	void SetRigidDynamicLockFlag(std::string name, int axis, bool st);
	void SetAngularDamping(std::string name, float ad);
	void SetLinearVelocity(std::string name, PxFloat3 v);
	void SetKinematicFlag(std::string name, bool st);
	void SetKinematicTarget(std::string name, PxFloat3& pos, PxFloat4& quat);

	// 刚体通用方法
	void DeletePxRigidDynamic(std::string name);
	void DeletePxRigidStatic(std::string name);

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
