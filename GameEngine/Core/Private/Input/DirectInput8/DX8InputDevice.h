#pragma once
#include "Common.h"

class DX8InputDevice : public IInputDevice
{
private:
    IDirectInput8* Input;  	
	IDirectInputDevice8* KeyboardDevice;
	IDirectInputDevice8* MouseDevice;
	array<BYTE, 256> PreviusKeys;
	array<EPressingState, 256> KeyStates;
	DIMOUSESTATE2 PreviusMouseState;
	array<EPressingState, 3> MouseButtonState;
	int2 MousePosition;
	int2 MouseMove;

public:
	void Initialize(HWND HWnd) override;
	void Finalize() override;
	void Update() override;
	EPressingState* GetKeyStates() override;
	EPressingState* GetMouseButtonStates() override;
	int2 GetMousePosition();
	int2 GetMouseMove();
};
