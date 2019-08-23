#pragma once

#include "Manager/GameObject.h"

class Cube :
	public GameObject
{
public:
	Cube(const Transform& transform = Transform(), const std::string& name = "");
	~Cube();

	void GetPicked(const float& dst, const DirectX::XMFLOAT3& hitPoint);

	void TurnOnBlink();
	void TurnOffBlink();
	void SetColor(DirectX::XMFLOAT3 color);
	DirectX::XMFLOAT3 GetColor();

private:
	virtual void Update(const GameTimer& gt)override;
	virtual void Release()override;

public:

	Int3 mMapIndex;

private:
	bool mBlink = false;
	bool mTurnWhite = true;
};