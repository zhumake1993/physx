#pragma once

#include "Manager/GameObject.h"

class Fragment :
	public GameObject
{
public:
	Fragment(const Transform& transform = Transform(), const std::string& name = "");
	~Fragment();

	void SetColor(DirectX::XMFLOAT3 color);

private:
	virtual void Update(const GameTimer& gt)override;
	virtual void Release()override;

public:
	//

private:
	std::string mMatName;
};