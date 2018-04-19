#pragma once
#include "..\UIControl.h"

// BindControl : スクロールに影響するコントロールを包括したコントロール

class FUIScrollBar : public FUIControl
{
private:
	SUIViewRange* TargetRange;
	intrect BarRectangle;
	EUIColorType BarColor;
	int MouseDownPositionY;
	int MouseDownBeginIndex;

private:
	void SetTopIndex(int Index);
	void OnRendering() override;
	void OnMouseDown(EMouseButton Button, int2 MousePosition) override;
	void OnMouseUp(EMouseButton Button, int2 MousePosition) override;
	void OnMouseMove(int2 MousePosition) override;
	void OnMouseLeave(int2 MousePosition) override;

public:
	FUIScrollBar();
	void SetTargetRange(SUIViewRange* InTargetRange) { TargetRange = InTargetRange; }
	void Scroll(int Delta);
};