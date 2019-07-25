#pragma once

#include "Manager/GameObject.h"

class Globe :
	public GameObject
{
public:
	Globe(const std::string& name, const Transform& transform = Transform());
	~Globe();

private:
	virtual void Update()override;

public:


private:

};