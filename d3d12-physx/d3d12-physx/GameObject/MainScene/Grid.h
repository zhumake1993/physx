#pragma once

#include "Manager/GameObject.h"

class Grid :
	public GameObject
{
public:
	Grid(const Transform& transform = Transform(), const std::string& name = "");
	~Grid();

private:
	virtual void Update(const GameTimer& gt)override;

public:


private:

};