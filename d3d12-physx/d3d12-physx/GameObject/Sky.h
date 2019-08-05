#pragma once

#include "Manager/GameObject.h"

class Sky :
	public GameObject
{
public:
	Sky(const Transform& transform = Transform(), const std::string& name = "");
	~Sky();

private:
	virtual void Update(const GameTimer& gt)override;

public:


private:

};