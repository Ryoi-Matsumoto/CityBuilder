#pragma once
#include <Windows.h>

struct SUIWinAPIMessage
{
	uint Message;
	WPARAM WParameter;
	LPARAM LParameter;

	wchar_t GetChar() const
	{
		return WParameter;
	}

	int2 GetInt2() const
	{
		return int2(LParameter & 0xffff, (LParameter >> 16) & 0xffff);
	}

	int GetWheelDelta() const
	{
		return GET_WHEEL_DELTA_WPARAM(WParameter) / WHEEL_DELTA;
	}
};