#pragma once

#include <string>

#include "../physx/Common/PhysXCommon.h"

class PhysX
{
public:
	PhysX();
	~PhysX();

	void InitPhysics();

	// ≥°æ∞
	void CreateScene();
	void CleanupScene();

	// ∏’ÃÂ
	void CreatePxRigidStatic(std::string name, void* pdesc);
	void CreatePxRigidDynamic(std::string name, void* pdesc);

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
