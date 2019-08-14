#pragma once

#include "Manager/GameObject.h"

class Character :
	public GameObject
{
public:
	Character(const Transform& transform = Transform(), const std::string& name = "");
	~Character();

private:
	virtual void Update(const GameTimer& gt)override;

	// Ðý×ª
	void Pitch(float angle);
	void RotateY(float angle);

	// Ñ¡È¡
	void Pick(int sx, int sy);

public:

	float mLateralSpeed = 5.0f;
	float mVerticalSpeed = 5.0f;

private:

	float mFallTime = 0.0f;
	const float mGravity = -9.8f;
	float mStartVerticalSpeed = 0.0f;
	float mCurrVerticalSpeed = 0.0f;
	POINT mLastMousePos;
};