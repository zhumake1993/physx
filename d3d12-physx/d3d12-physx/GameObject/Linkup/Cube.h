#pragma once

#include "Manager/GameObject.h"

class Cube :
	public GameObject
{
public:
	Cube(const Transform& transform = Transform());
	~Cube();

	virtual void GetPicked(float dst, DirectX::XMFLOAT3 hitPoint)override;

	void TurnOnBlink();
	void TurnOffBlink();
	void SetColor(DirectX::XMFLOAT3 color);
	DirectX::XMFLOAT3 GetColor();

private:
	virtual void Update(const GameTimer& gt)override;

public:

	Int3 mMapIndex;

private:
	bool mBlink = false;
	bool mTurnWhite = true;
};