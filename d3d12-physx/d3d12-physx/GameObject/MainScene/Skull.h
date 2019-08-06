#pragma once

#include "Manager/GameObject.h"

class Skull :
	public GameObject
{
public:
	Skull(const Transform& transform = Transform(), const std::string& name = "");
	~Skull();

private:
	virtual void Update(const GameTimer& gt)override;

public:


private:

};