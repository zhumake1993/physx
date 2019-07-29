#include "InputManager.h"

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

void InputManager::Update(const GameTimer& gt)
{
	for (int i = 0; i < size; i++) {
		mKeyDown[i] = false;
		mKeyUp[i] = false;
	}

	for (int i = 0; i < 3; i++) {
		mMouseDown[i] = false;
		mMouseUp[i] = false;
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

void InputManager::OnMouseDown(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0) {
		if (mMousePress[0] == false) {
			mMouseDown[0] = true;
			mMousePress[0] = true;
		}
	}
	else if ((btnState & MK_RBUTTON) != 0) {
		if (mMousePress[1] == false) {
			mMouseDown[1] = true;
			mMousePress[1] = true;
		}
	}
	else if ((btnState & MK_MBUTTON) != 0) {
		if (mMousePress[2] == false) {
			mMouseDown[2] = true;
			mMousePress[2] = true;
		}
	}
	mX = x;
	mY = y;
}

void InputManager::OnMouseUp(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0) {
		mMouseUp[0] = true;
		mMousePress[0] = false;
	}
	else if ((btnState & MK_RBUTTON) != 0) {
		mMouseUp[1] = true;
		mMousePress[1] = false;
	}
	else if ((btnState & MK_MBUTTON) != 0) {
		mMouseUp[2] = true;
		mMousePress[2] = false;
	}
	mX = x;
	mY = y;
}

void InputManager::OnMouseMove(WPARAM btnState, int x, int y)
{
	mX = x;
	mY = y;
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

bool InputManager::GetMouseDown(int key)
{
	return mMouseDown[key];
}

bool InputManager::GetMousePress(int key)
{
	return mMousePress[key];
}

bool InputManager::GetMouseUp(int key)
{
	return mMouseUp[key];
}

int InputManager::GetMouseX()
{
	return mX;
}

int InputManager::GetMouseY()
{
	return mY;
}

void InputManager::Clear()
{
	for (int i = 0; i < size; i++) {
		mKeyDown[i] = false;
		mKeyPress[i] = false;
		mKeyUp[i] = false;
	}

	for (int i = 0; i < 3; i++) {
		mMouseDown[i] = false;
		mMousePress[i] = false;
		mMouseUp[i] = false;
	}

	mX = 0;
	mY = 0;
}
