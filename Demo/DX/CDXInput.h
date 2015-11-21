#pragma once

#include <dinput.h>
#include "../GameConfig.h"

class CDXInput
{
private:
	IDirectInput8* pDInputObject = 0;

	IDirectInputDevice8* pMouse = 0;
	DIMOUSESTATE previousMouseState;
	DIMOUSESTATE currentMouseState;

	IDirectInputDevice8* pKeyboard = 0;
	char previousKeyboardState[256];
	char currentKeyboardState[256];

public:
	~CDXInput();

	void Initialize(HINSTANCE hInstance, HWND wnd);

	void Update();
	void Change();

	bool KeyDown(int key);
	bool KeyPress(int key);

	bool MouseButtonDown(int button);
	float GetMouseX() { return (float)this->currentMouseState.lX; }
	float GetMouseY() { return (float)this->currentMouseState.lY; }
	float GetMouseZ() { return (float)this->currentMouseState.lZ; }
};