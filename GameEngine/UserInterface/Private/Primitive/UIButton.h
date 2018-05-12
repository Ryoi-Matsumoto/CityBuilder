#pragma once
#include "..\UIControl.h"
#include "Rendering\Public\RTextBlock.h"
#include "UITextBlock.h"

class FUIButton : public FUIControl
{
private:
	SUIActiveColor ActiveColor;
	SUIEvent<> OnClicked;
	bool IsChangingBackColor;

protected:
	EUIColorType CurrentColor;

private:
	void SetColor(EUIColorType Color)
	{
		CurrentColor = Color;
		if (IsChangingBackColor)
			BackColor = Color;
	}

	void OnInitialize() override
	{
		SetColor(ActiveColor.Normal);
	}

	void OnMouseMove(int2 MousePosition) override
	{
		if(!GetMouseButtonDown(EMouseButton::Left))
			SetColor(ActiveColor.Cursor);
		Update();
	}

	void OnMouseLeave(int2 MousePosition) override
	{
		SetColor(ActiveColor.Normal);
		Update();
	}

	void OnMouseDown(EMouseButton Button, int2 MousePosition) override
	{
		if (Button == EMouseButton::Left)
		{
			SetColor(ActiveColor.Selected);
			Update();
		}
	}

	void OnMouseUp(EMouseButton Button, int2 MousePosition) override
	{
		if (Button == EMouseButton::Left)
		{
			SetColor(ActiveColor.Cursor);
			Update();
			if(OnClicked)
				OnClicked();
		}
	}

public:
	FUIButton(bool IsChangingBackColor) : IsChangingBackColor(IsChangingBackColor) {}

	void SetActiveColor(SUIActiveColor InActiveColor) { ActiveColor = InActiveColor; }
	void SetEventOnClicked(SUIEvent<> InOnClicked) { OnClicked = InOnClicked; }
};

class FUITriangle : public FUIButton
{
private:
	bool Upside;

private:
	void OnRendering() override
	{
		auto TriangleColor = GetApplication()->GetColor(CurrentColor);
		GetRenderer()->DrawTriangle(intrect(0, GetRenderingSize()), TriangleColor, Upside);
	}

public:
	FUITriangle() : FUIButton(false) {}

	void SetUpside(bool InUpside) { Upside = InUpside; }
};

class FUITextButton : public FUIButton
{
private:
	FUITextBlock* TextBlock;

private:
	void OnInitialize() override
	{
		AddChild(TextBlock);
	}

	uint GetActualWidth() const override { return TextBlock->GetWidth(); }
	uint GetActualHeight() const override { return TextBlock->GetHeight(); }

public:
	FUITextButton() : FUIButton(true), TextBlock(new FUITextBlock()) 
	{
		TextBlock->SetBackColor(EUIColorType::None);
		SetActiveColor(SUIActiveColor(EUIColorType::Base, EUIColorType::ButtonOnCursor, EUIColorType::ButtonOnClick));
	}

	void SetText(wstring Text) { TextBlock->SetText(Text); }
	//void SetTextBackColor(EUIColorType Color) { TextBlock->SetBackColor(Color); }
};