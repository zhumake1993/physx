#pragma once

#include "Manager/GameObject.h"

class Sphere :
	public GameObject
{
public:
	Sphere(const Transform& transform = Transform(), const std::string& name = "");
	~Sphere();

private:
	virtual void Update(const GameTimer& gt)override;

public:


private:

};