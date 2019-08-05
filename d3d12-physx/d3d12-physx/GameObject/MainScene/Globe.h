#pragma once

#include "Manager/GameObject.h"

class Globe :
	public GameObject
{
public:
	Globe(const Transform& transform = Transform(), const std::string& name = "");
	~Globe();

private:
	virtual void Update(const GameTimer& gt)override;

public:


private:

};