#include "GameObject.h"

#include "InputManager.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

XMFLOAT4X4 GameObject::GetWorld()
{
	//XMFLOAT4X4 world;

	//XMVECTOR S = XMLoadFloat3(&mScale);
	//XMVECTOR P = XMLoadFloat3(&mTranslation);
	//XMVECTOR Q = XMLoadFloat4(&mRotationQuat);

	//XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	//XMStoreFloat4x4(&world, XMMatrixAffineTransformation(S, zero, Q, P));

	//return world;
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

void GameObject::AddRigidBody()
{
}

void GameObject::Update()
{
}
