#pragma once

#include "Manager/GameObject.h"

class RenderAndFilter :
	public GameObject
{
public:
	RenderAndFilter(const Transform& transform = Transform(), const std::string& name = "");
	~RenderAndFilter();

private:
	virtual void Update(const GameTimer& gt)override;

public:
	//

private:
	//
};