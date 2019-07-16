#pragma once

#include "Common/d3dUtil.h"

const int size = 256;

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
	bool mKeyDown[size];
	bool mKeyPress[size];
	bool mKeyUp[size];
};

extern std::unique_ptr<InputManager> gInputManager;