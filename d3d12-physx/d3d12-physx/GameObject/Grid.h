#pragma once

#include "Manager/GameObject.h"

class Grid :
	public GameObject
{
public:
	Grid(const std::string& name, const Transform& transform = Transform());
	~Grid();

private:
	virtual void Update()override;

public:


private:

};