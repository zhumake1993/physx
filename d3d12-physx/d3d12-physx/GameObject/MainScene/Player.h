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

	// ��ת
	void Pitch(float angle);
	void RotateY(float angle);

	// ƽ��
	void Strafe(float d);
	void Walk(float d);
	void Fly(float d);

public:


private:

	POINT mLastMousePos;
};