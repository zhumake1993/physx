#pragma once

#include "Manager/GameObject.h"

class Logic :
	public GameObject
{
public:
	Logic(const std::string& name, const Transform& transform = Transform());
	~Logic();

private:
	virtual void Update(const GameTimer& gt)override;

public:
	std::string mLastPickCube = "";

private:
	//
};