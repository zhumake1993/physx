#pragma once

#include "Manager/GameObject.h"

class Cube :
	public GameObject
{
public:
	Cube(const std::string& name, const Transform& transform = Transform());
	~Cube();

private:
	virtual void Update()override;

public:


private:

};