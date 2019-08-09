#pragma once

#include "Manager/GameObject.h"

class Test :
	public GameObject
{
public:
	Test(const Transform& transform = Transform(), const std::string& name = "");
	~Test();

private:
	virtual void Update(const GameTimer& gt)override;

	void Move(const GameTimer& gt);

	// Ðý×ª
	void Pitch(float angle);
	void RotateY(float angle);

public:


private:

	POINT mLastMousePos;
};