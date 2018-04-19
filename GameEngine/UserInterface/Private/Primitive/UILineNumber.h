#pragma once
#include "..\UIControl.h"
#include "UIScrollBar.h"

class FUILineNumber : public FUIControl
{
private:
	SUIViewRange* Range;
	FRFontLetter* NumberFonts[10];
	uint Width;
	int2 FontSize;

private:
	void OnInitialize(FUIApplication* Application) override;
	void OnRendering() override;

public:
	void SetViewRange(SUIViewRange* InRange) { Range = InRange; }
	uint GetActualWidth() const override { return Width; }
};