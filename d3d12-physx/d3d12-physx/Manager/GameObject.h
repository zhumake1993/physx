#pragma once

#include "Common/d3dUtil.h"
#include "Common/Camera.h"
#include "InstanceManager.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "InputManager.h"

using namespace DirectX;

class GameObject
{
	friend class GameObjectManager;

public:
	GameObject();
	virtual ~GameObject();

protected:
	bool GetKeyDown(int key);
	bool GetKeyPress(int key);
	bool GetKeyUp(int key);

private:
	virtual void Update();

	XMFLOAT4X4 GetWorld();

public:
	std::string mGameObjectName = "GameObject";

	XMFLOAT3 mTranslation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 mRotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 mScale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	std::string mMatName;
	XMFLOAT4X4 mTexTransform = MathHelper::Identity4x4();

	std::string mMeshName;

	int mRenderLayer = -1;

	bool mReceiveShadow = true;
};