#pragma once
#include "..\Container\UIScrollView.h"
#include "Rendering\Public\RTextBlock.h"
#include "UIScrollBar.h"

class FUITextBlock : public FUIControl, public FUIScrollable
{
protected:
	FRTextBlock* TextBlock;
	float4 FontColor;
	wstring StartupText;

protected:
	void OnInitialize() override;
	void OnAreaChanged() override;
	void OnRendering() override;
	uint GetActualWidth() const override { return TextBlock->GetWidth(); }
	uint GetActualHeight() const override { return TextBlock->GetHeight(); }

public:
	FUITextBlock();

	void SetText(wstring Text);
	wstring GetText() const;
};