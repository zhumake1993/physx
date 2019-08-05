#pragma once

#include "Manager/GameObject.h"

class Cylinder :
	public GameObject
{
public:
	Cylinder(const Transform& transform = Transform(), const std::string& name = "");
	~Cylinder();

private:
	virtual void Update(const GameTimer& gt)override;

public:


private:

};