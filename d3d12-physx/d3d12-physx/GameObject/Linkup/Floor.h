#pragma once

#include "Manager/GameObject.h"

class Floor :
	public GameObject
{
public:
	Floor(const std::string& name, const Transform& transform = Transform());
	~Floor();

private:
	virtual void Update()override;

public:


private:

};