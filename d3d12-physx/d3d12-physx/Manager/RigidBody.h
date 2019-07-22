#pragma once

#include "Common/d3dUtil.h"

using namespace DirectX;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

protected:
	XMFLOAT4X4 GetWorld();

	bool GetKeyDown(int key);
	bool GetKeyPress(int key);
	bool GetKeyUp(int key);

	virtual void AddRigidBody();

	virtual void Update();

public:
	std::string mName = "GameObject";
	Transform mTransform;
};