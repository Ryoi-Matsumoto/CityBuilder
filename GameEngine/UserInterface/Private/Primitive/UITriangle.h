#pragma once
#include "..\UIControl.h"

class FUITriangle : public FUIControl
{
private:
	bool Upside;
	SUIActiveColor ActiveColor;
	EUIColorType Color;
	SUIEvent<> OnClicked;

private:
	void OnInitialize(FUIApplication* Application) override
	{
		Color = ActiveColor.Normal;
	}

	void OnMouseMove(int2 MousePosition) override
	{
		if(!GetMouseButtonDown(EMouseButton::Left))
			Color = ActiveColor.Cursor;
		Update();
	}

	void OnMouseLeave(int2 MousePosition) override
	{
		Color = ActiveColor.Normal;
		Update();
	}

	void OnMouseDown(EMouseButton Button, int2 MousePosition) override
	{
		if (Button == EMouseButton::Left)
		{
			Color = ActiveColor.Selected;
			Update();
		}
	}

	void OnMouseUp(EMouseButton Button, int2 MousePosition) override
	{
		if (Button == EMouseButton::Left)
		{
			Color = ActiveColor.Cursor;
			Update();
			OnClicked();
		}
	}

	void OnRendering() override 
	{ 
		auto TriangleColor = GetWindow()->GetApplication()->GetColor(Color);
		Renderer->DrawTriangle(intrect(0, GetRenderingSize()), TriangleColor, Upside);
	}

public:
	void SetActiveColor(SUIActiveColor InActiveColor) { ActiveColor = InActiveColor; }
	void SetUpside(bool InUpside) { Upside = InUpside; }
	void SetEventOnClicked(SUIEvent<> InOnClicked) { OnClicked = InOnClicked; }
};