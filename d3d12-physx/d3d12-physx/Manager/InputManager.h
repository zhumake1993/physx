#pragma once

#include "Common/d3dUtil.h"

class InputManager
{
public:
	InputManager();
	~InputManager();

	void Initialize();

	void Update();

	void OnKeyDown(WPARAM key);
	void OnKeyUp(WPARAM key);

	bool GetKeyDown(WPARAM key);
	bool GetKeyPress(WPARAM key);
	bool GetKeyUp(WPARAM key);

private:
	void Clear();

public:
	//

private:
	static const int size = 256;
	bool mKeyDown[size];
	bool mKeyPress[size];
	bool mKeyUp[size];
};