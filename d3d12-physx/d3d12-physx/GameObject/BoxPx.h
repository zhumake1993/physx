#pragma once

#include "Manager/GameObject.h"

class BoxPx :
	public GameObject
{
public:
	BoxPx(const std::string& name, const Transform& transform = Transform());
	~BoxPx();

private:
	virtual void Update()override;

public:


private:

};