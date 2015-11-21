#include "CDXInput.h"

void CDXInput::Initialize(HINSTANCE hInstance, HWND wnd)
{
	HR(DirectInput8Create(
		hInstance,
		DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&this->pDInputObject, 0));

	ZeroMemory(&this->currentMouseState, sizeof(this->currentMouseState));

	HR(this->pDInputObject->CreateDevice(
		GUID_SysMouse, &this->pMouse, 0));
	HR(this->pMouse->SetDataFormat(&c_dfDIMouse));
	HR(this->pMouse->SetCooperativeLevel(wnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND));
	HR(this->pMouse->Acquire());

	ZeroMemory(this->currentKeyboardState, sizeof(this->currentKeyboardState));

	HR(this->pDInputObject->CreateDevice(
		GUID_SysKeyboard, &this->pKeyboard, 0));
	HR(this->pKeyboard->SetDataFormat(
		&c_dfDIKeyboard));
	HR(this->pKeyboard->SetCooperativeLevel(
		wnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND));
	HR(this->pKeyboard->Acquire());
}

CDXInput::~CDXInput()
{
	SAFE_RELEASE(this->pDInputObject);

	this->pMouse->Unacquire();
	SAFE_RELEASE(this->pMouse);

	this->pKeyboard->Unacquire();
	SAFE_RELEASE(this->pKeyboard);
}

void CDXInput::Update()
{
	this->previousMouseState = this->currentMouseState;
	HRESULT hr = this->pMouse->GetDeviceState(
		sizeof(DIMOUSESTATE), (void**)&this->currentMouseState);
	if (FAILED(hr))
	{
		ZeroMemory(&this->currentMouseState, sizeof(this->currentMouseState));
		hr = this->pMouse->Acquire();
	}

	for (int i = 0;i < 256;i++)
		if (this->KeyDown(this->currentKeyboardState[i]))
			this->previousKeyboardState[i] = this->currentKeyboardState[i];

	hr = this->pKeyboard->GetDeviceState(
		sizeof(this->currentKeyboardState), (void**)&this->currentKeyboardState);
	if (FAILED(hr))
	{
		ZeroMemory(this->currentKeyboardState, sizeof(this->currentKeyboardState));
		hr = this->pKeyboard->Acquire();
	}
}

void CDXInput::Change()
{
	for (int i = 0;i < 256;i++)
		this->previousKeyboardState[i] = this->currentKeyboardState[i];
}

bool CDXInput::KeyDown(int key)
{
	return (this->currentKeyboardState[key] & 0x80);
}

bool CDXInput::KeyPress(int key)
{
	return (!(this->previousKeyboardState[key] & 0x80) &&
		this->currentKeyboardState[key] & 0x80);
}

bool CDXInput::MouseButtonDown(int button)
{
	return (this->currentMouseState.rgbButtons[button] & 0x80);
}