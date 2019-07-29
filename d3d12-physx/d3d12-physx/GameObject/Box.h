#pragma once

#include "Manager/GameObject.h"

class Box :
	public GameObject
{
public:
	Box(const std::string& name, const Transform& transform = Transform());
	~Box();

private:
	virtual void Update(const GameTimer& gt)override;

public:


private:

};