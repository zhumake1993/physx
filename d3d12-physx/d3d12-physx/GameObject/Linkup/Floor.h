#pragma once

#include "Manager/GameObject.h"

class Floor :
	public GameObject
{
public:
	Floor(const Transform& transform = Transform(), const std::string& name = "");
	~Floor();

private:
	virtual void Update(const GameTimer& gt)override;

public:


private:

};