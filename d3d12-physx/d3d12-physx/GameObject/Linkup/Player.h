#pragma once

#include "Manager/GameObject.h"

class Player :
	public GameObject
{
public:
	Player(const Transform& transform = Transform(), const std::string& name = "");
	~Player();

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