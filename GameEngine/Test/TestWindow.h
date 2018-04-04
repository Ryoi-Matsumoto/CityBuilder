#pragma once
#include <Windows.h>

class FTestWindow
{
private:
	HWND HWindow;
	const uint Width = 1280;
	const uint Height = 700;

public:
	FTestWindow();
	bool Update();
	HWND GetHWindow() const { return HWindow; }
	uint GetHeight() const { return Height; }
	uint GetWidth() const { return Width; }
	int2 GetSize() const { return int2(Width, Height); }
};