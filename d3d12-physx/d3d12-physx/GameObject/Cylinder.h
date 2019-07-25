#pragma once

#include "Manager/GameObject.h"

class Cylinder :
	public GameObject
{
public:
	Cylinder(const std::string& name, const Transform& transform = Transform());
	~Cylinder();

private:
	virtual void Update()override;

public:


private:

};