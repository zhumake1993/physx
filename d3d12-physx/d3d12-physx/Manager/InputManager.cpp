#include "InputManager.h"

std::unique_ptr<InputManager> gInputManager = std::make_unique<InputManager>();

InputManager::InputManager()
{
}

InputManager::~InputManager()
{
}

void InputManager::Initialize()
{
	Clear();
}

void InputManager::Update()
{
	for (int i = 0; i < size; i++) {
		mKeyDown[i] = false;
		mKeyUp[i] = false;
	}
}

void InputManager::OnKeyDown(WPARAM key)
{
	if (mKeyPress[key] == false) {
		mKeyDown[key] = true;
		mKeyPress[key] = true;
	}
}

void InputManager::OnKeyUp(WPARAM key)
{
	mKeyUp[key] = true;
	mKeyPress[key] = false;
}

bool InputManager::GetKeyDown(WPARAM key)
{
	return mKeyDown[key];
}

bool InputManager::GetKeyPress(WPARAM key)
{
	return mKeyPress[key];
}

bool InputManager::GetKeyUp(WPARAM key)
{
	return mKeyUp[key];
}

void InputManager::Clear()
{
	for (int i = 0; i < size; i++) {
		mKeyDown[i] = false;
		mKeyPress[i] = false;
		mKeyUp[i] = false;
	}
}
