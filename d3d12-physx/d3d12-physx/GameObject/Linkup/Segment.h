#pragma once

#include "Manager/GameObject.h"

class Segment :
	public GameObject
{
public:
	Segment(const std::string& name, const Transform& transform = Transform());
	~Segment();

private:
	virtual void Update(const GameTimer& gt)override;

public:
	//

private:
	//DirectX::XMFLOAT4X4 mlook;
	DirectX::XMFLOAT4 mQuat;
};