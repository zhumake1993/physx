#pragma once

#include "Manager/GameObject.h"

class Fragment :
	public GameObject
{
public:
	Fragment(const Transform& transform = Transform());
	~Fragment();

	void SetColor(DirectX::XMFLOAT3 color);

private:
	virtual void Update(const GameTimer& gt)override;

public:
	//

private:
	//
};