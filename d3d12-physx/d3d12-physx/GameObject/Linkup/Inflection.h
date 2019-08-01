#pragma once

#include "Manager/GameObject.h"

class Inflection :
	public GameObject
{
public:
	Inflection(const Transform& transform = Transform());
	~Inflection();

private:
	virtual void Update(const GameTimer& gt)override;

public:
	//

private:
	//
};