#pragma once
#include "UITextBlock.h"

struct SUIActiveColor
{
	float4 Normal;
	float4 Cursor;
	float4 Selected;

	SUIActiveColor(float4 Normal, float4 Cursor, float4 Selected)
		: Normal(Normal)
		, Cursor(Cursor)
		, Selected(Selected)
	{
	}
};

class FUIButton : public FUIControlContainer
{
private:
	bool IsPressed;

private:
	void OnMouseDown(EMouseButton Button, int2 MousePosition) override;
	void OnMouseUp(EMouseButton Button, int2 MousePosition) override;
	void OnMouseLeave(int2 MousePosition) override;
	void OnMouseMove(int2 MousePosition) override;

public:
	FUIButton();
	
	UIAttribute(SUIActiveColor, ActiveColor)
	UIAttribute(TUIEvent<>, OnClicked)
};

class FUITextButton : public FUIButton
{
private:
	void OnInitialize(FUIApplication* Application) override;

public:
	FUITextButton();

	UIAttribute(wstring, Text)
};