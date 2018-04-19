#include "UIButton.h"

FUIButton::FUIButton()
	: ActiveColor(EUIColorType::Button, EUIColorType::ButtonOnCursor, EUIColorType::ButtonOnClick)
	, IsPressed(false)
{
}

void FUIButton::OnMouseMove(int2 MousePosition)
{
	BackColor = ActiveColor.Cursor;
	Update();
}

void FUIButton::OnMouseLeave(int2 MousePosition)
{
	BackColor = ActiveColor.Normal;
	Update();
}

void FUIButton::OnMouseDown(EMouseButton Button, int2 MousePosition)
{
	BackColor = ActiveColor.Selected;
	Update();	
	if (IsPressed && OnClicked)
		OnClicked();
	IsPressed = true;
}

void FUIButton::OnMouseUp(EMouseButton Button, int2 MousePosition)
{	
	BackColor = ActiveColor.Cursor;
	Update();
	IsPressed = false;
}