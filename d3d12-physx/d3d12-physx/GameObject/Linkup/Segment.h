#pragma once

#include "Manager/GameObject.h"

class Segment :
	public GameObject
{
public:
	Segment(const Transform& transform = Transform(), const std::string& name = "");
	~Segment();

private:
	virtual void Update(const GameTimer& gt)override;

public:
	//

private:
	//DirectX::XMFLOAT4X4 mlook;
	DirectX::XMFLOAT4 mQuat;
};