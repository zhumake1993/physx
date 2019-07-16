#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

bool GameObject::GetKeyDown(int key)
{
	return gInputManager->GetKeyDown(key);
}

bool GameObject::GetKeyPress(int key)
{
	return gInputManager->GetKeyPress(key);
}

bool GameObject::GetKeyUp(int key)
{
	return gInputManager->GetKeyUp(key);
}

void GameObject::Update()
{
}

XMFLOAT4X4 GameObject::GetWorld()
{
	XMMATRIX T = XMMatrixTranslation(mTranslation.x, mTranslation.y, mTranslation.z);
	XMMATRIX R = XMMatrixRotationX(mRotation.x) * XMMatrixRotationY(mRotation.y) * XMMatrixRotationZ(mRotation.z);
	XMMATRIX S = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	XMMATRIX W = S * R * T; // ×¢ÒâË³Ðò
	XMFLOAT4X4 mWorld;
	XMStoreFloat4x4(&mWorld, W);
	return mWorld;
}

