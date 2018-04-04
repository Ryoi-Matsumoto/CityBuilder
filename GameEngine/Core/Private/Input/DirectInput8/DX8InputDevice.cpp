#include "DX8InputDevice.h"

void DX8InputDevice::Initialize(HWND HWnd)
{
	HR(DirectInput8Create
	(
		GetModuleHandle(nullptr),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&Input,
		nullptr
	));

	// キーボード
	HR(Input->CreateDevice(GUID_SysKeyboard, &KeyboardDevice, nullptr));
	HR(KeyboardDevice->SetDataFormat(&c_dfDIKeyboard));
	HR(KeyboardDevice->SetCooperativeLevel(HWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND));
	KeyboardDevice->Acquire();

	// マウス
	HR(Input->CreateDevice(GUID_SysMouse, &MouseDevice, nullptr));	
	HR(MouseDevice->SetDataFormat(&c_dfDIMouse2));	
	HR(MouseDevice->SetCooperativeLevel(HWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND));

	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	// 相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）
	HR(MouseDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph));

	MouseDevice->Acquire();
}

void DX8InputDevice::Finalize()
{
	SafeRelease(KeyboardDevice);	
	SafeRelease(MouseDevice);
	SafeRelease(Input);
}

void DX8InputDevice::Update()
{
	// キーボード
	array<BYTE, 256> Keys;

	if (FAILED(KeyboardDevice->GetDeviceState(sizeof(Keys), &Keys)))
	{
			while (KeyboardDevice->Acquire() == DIERR_INPUTLOST);
	}

	for (int i = 0; i < 256; i++)
	{
		if (PreviusKeys[i] != Keys[i])
		{
			if (Keys[i] & 0x80)KeyStates[i] = EPressingState::Down;
			else KeyStates[i] = EPressingState::Up;
		}
		else if (Keys[i] & 0x80)KeyStates[i] = EPressingState::Pressed;
		else KeyStates[i] = EPressingState::None;
	}
	PreviusKeys = Keys;

	// マウス
	DIMOUSESTATE2 MouseState;
	
	if (FAILED(MouseDevice->GetDeviceState(sizeof(MouseState), &MouseState)))
	{
		while (KeyboardDevice->Acquire() == DIERR_INPUTLOST);
	}
	
	for (int i = 0; i < 3; i++)
	{
		if (PreviusMouseState.rgbButtons[i] != MouseState.rgbButtons[i])
		{
			if (MouseState.rgbButtons[i] & 0x80)MouseButtonState[i] = EPressingState::Down;
			else MouseButtonState[i] = EPressingState::Up;
		}
		else if (MouseState.rgbButtons[i] & 0x80)MouseButtonState[i] = EPressingState::Pressed;
		else MouseButtonState[i] = EPressingState::None;
	}
	POINT CursorPosition;
	GetCursorPos(&CursorPosition);
	MousePosition = int2(CursorPosition.x, CursorPosition.y);
	MouseMove = int2(MouseState.lX, MouseState.lY);
	PreviusMouseState = MouseState;	
}

EPressingState* DX8InputDevice::GetKeyStates()
{
	return &KeyStates[0];
}

EPressingState* DX8InputDevice::GetMouseButtonStates()
{
	return &MouseButtonState[0];
}

int2 DX8InputDevice::GetMousePosition()
{
	return MousePosition;
}

int2 DX8InputDevice::GetMouseMove()
{
	return MouseMove;
}