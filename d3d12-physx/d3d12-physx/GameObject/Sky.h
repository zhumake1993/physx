#pragma once

#include "Manager/GameObject.h"

class Sky :
	public GameObject
{
public:
	Sky(const std::string& name, const Transform& transform = Transform());
	~Sky();

private:
	virtual void Update(const GameTimer& gt)override;

public:


private:

};