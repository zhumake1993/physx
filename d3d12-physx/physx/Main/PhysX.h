#pragma once

#include <string>

class PhysX
{
public:
	PhysX();
	~PhysX();

	void InitPhysics();
	void CreateScene();

	void CreatePxRigidStatic();
	void CreatePxRigidDynamic(std::string name, void* pdesc);

	void Update(float delta);

	void Get(float& x, float& y, float& z, float& a, float& b, float& c, float& d);

	void CleanupPhysics();

private:
	//

public:
	//

private:
	//
};
