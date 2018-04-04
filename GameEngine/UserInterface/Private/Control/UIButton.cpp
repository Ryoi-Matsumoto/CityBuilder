#include "../../Public/Control/UIButton.h"
#include "../../Public/Control/UITextBlock.h"

FUIButton::FUIButton()
	: _ActiveColor(float4(float3(0), 0), float4(float3(0), 0.3), float4(float3(0), 0.6))
	, IsPressed(false)
{
	_Margin = 5;	
}

void FUIButton::OnMouseMove(int2 MousePosition)
{
	_BackColor = _ActiveColor.Cursor;
	Update();
};

void FUIButton::OnMouseLeave(int2 MousePosition)
{
	_BackColor = _ActiveColor.Normal;
	Update();
};

void FUIButton::OnMouseDown(EMouseButton Button, int2 MousePosition)
{
	_BackColor = _ActiveColor.Selected;
	Update();	
	if (IsPressed)_OnClicked();
	IsPressed = true;
};

void FUIButton::OnMouseUp(EMouseButton Button, int2 MousePosition)
{	
	_BackColor = _ActiveColor.Cursor;
	Update();
	IsPressed = false;
};

FUITextButton::FUITextButton()
	: _Text(L"Button")
{
}

void FUITextButton::OnInitialize(FUIApplication* Application)
{
	UIBase
	[
		UINew(FUITextBlock)
		.Text(_Text)
	];
}