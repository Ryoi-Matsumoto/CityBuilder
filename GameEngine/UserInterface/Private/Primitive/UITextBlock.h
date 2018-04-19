#pragma once
#include "..\Container\UIScrollView.h"
#include "Rendering\Public\RTextBlock.h"
#include "UIScrollBar.h"

class FUITextBlock : public FUIControl
{
protected:
	FRTextBlock* TextBlock;
	float4 FontColor;
	wstring StartupText;
	SUIViewRange ViewRange;

protected:
	void OnInitialize(FUIApplication* Application) override;
	void OnAreaChanged() override;
	void OnRendering() override;

public:
	FUITextBlock();

	void SetText(wstring Text);
	wstring GetText() const;
	SUIViewRange* GetViewRange() { return &ViewRange; }
};