#pragma once

#include "Manager/GameObject.h"

class CubeGreen :
	public GameObject
{
public:
	CubeGreen(const std::string& name, const Transform& transform = Transform());
	~CubeGreen();

	virtual void GetPicked(float dst, DirectX::XMFLOAT3 hitPoint)override;

	void TurnOffBlink();

private:
	virtual void Update(const GameTimer& gt)override;

public:


private:
	bool mBlink = false;
	bool mTurnWhite = true;
};