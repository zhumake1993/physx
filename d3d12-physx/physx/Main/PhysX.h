#pragma once

class PhysX
{
public:
	PhysX();
	~PhysX();

	void InitPhysics();
	void CreateObject();

	void Update(float delta);

	void Get(float& x, float& y, float& z, float& a, float& b, float& c, float& d);

	void cleanupPhysics();

private:
	//

public:
	//

private:
	//
};
