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
	void CreatePxRigidStatic();
	void CreatePxRigidDynamic(std::string name, void* pdesc);

	void Update(float delta);

	void GetPxRigidDynamicTransform(std::string name, PxFloat3& pos, PxFloat4& quat);
	void Get(float& x, float& y, float& z, float& a, float& b, float& c, float& d);

	void CleanupPhysics();

private:
	//

public:
	//

private:
	
	float mAccumulator = 0.0f;
	float mStepSize = 1.0f / 60.0f;
};
