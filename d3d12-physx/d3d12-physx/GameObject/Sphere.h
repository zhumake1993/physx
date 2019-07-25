#pragma once

#include "Manager/GameObject.h"

class Sphere :
	public GameObject
{
public:
	Sphere(const std::string& name, const Transform& transform = Transform());
	~Sphere();

private:
	virtual void Update()override;

public:


private:

};