#pragma once

#include "Manager/GameObject.h"

class Cube :
	public GameObject
{
public:
	Cube(const std::string& name, const Transform& transform = Transform());
	~Cube();

	virtual void GetPicked(float dst, DirectX::XMFLOAT3 hitPoint)override;

	void TurnOffBlink();

private:
	virtual void Update(const GameTimer& gt)override;

public:


private:
	bool mBlink = false;
	bool mTurnWhite = true;
};