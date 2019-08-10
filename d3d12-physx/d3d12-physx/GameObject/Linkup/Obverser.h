#pragma once

#include "Manager/GameObject.h"

class Obverser :
	public GameObject
{
public:
	Obverser(const Transform& transform = Transform(), const std::string& name = "");
	~Obverser();

private:
	virtual void Update(const GameTimer& gt)override;

	void Move(const GameTimer& gt);

	// Ðý×ª
	void Pitch(float angle);
	void RotateY(float angle);

	// Æ½ÒÆ
	void Strafe(float d);
	void Walk(float d);
	void Fly(float d);

public:


private:

	POINT mLastMousePos;
};