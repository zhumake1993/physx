#pragma once

#include "Common/d3dUtil.h"
#include "Common/GameTimer.h"

class InputManager
{
public:
	InputManager();
	~InputManager();

	void Initialize();

	void Update(const GameTimer& gt);

	void OnKeyDown(WPARAM key);
	void OnKeyUp(WPARAM key);

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	bool GetKeyDown(WPARAM key);
	bool GetKeyPress(WPARAM key);
	bool GetKeyUp(WPARAM key);

	bool GetMouseDown(int key);
	bool GetMousePress(int key);
	bool GetMouseUp(int key);
	int GetMouseX();
	int GetMouseY();

private:
	void Clear();

public:
	//

private:
	static const int size = 256;
	bool mKeyDown[size];
	bool mKeyPress[size];
	bool mKeyUp[size];

	bool mMouseDown[3];
	bool mMousePress[3];
	bool mMouseUp[3];
	int mX;
	int mY;
};