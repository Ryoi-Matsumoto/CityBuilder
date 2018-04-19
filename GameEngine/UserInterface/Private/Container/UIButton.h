#pragma once
#include "UIContainer.h"

class FUIButton : public FUIContainer
{
private:
	bool IsPressed;
	SUIActiveColor ActiveColor;
	SUIEvent<> OnClicked;

private:
	void OnMouseDown(EMouseButton Button, int2 MousePosition) override;
	void OnMouseUp(EMouseButton Button, int2 MousePosition) override;
	void OnMouseLeave(int2 MousePosition) override;
	void OnMouseMove(int2 MousePosition) override;

public:
	FUIButton();
	
	void SetActiveColor(SUIActiveColor InActiveColor) { ActiveColor = InActiveColor; }
	void SetEventOnClicked(SUIEvent<> InOnCliecked) { OnClicked = InOnCliecked; }
};