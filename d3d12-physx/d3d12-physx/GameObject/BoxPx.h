#pragma once

#include "Manager/GameObject.h"

class BoxPx :
	public GameObject
{
public:
	BoxPx(const Transform& transform = Transform());
	~BoxPx();

private:
	virtual void Update(const GameTimer& gt)override;

public:


private:

};