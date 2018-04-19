#include "UITextBlock.h"

FUITextBlock::FUITextBlock()
	: StartupText(L"TextBlock"), TextBlock(nullptr), FontColor(float4(1))
{
}

void FUITextBlock::OnInitialize(FUIApplication* Application)
{
	TextBlock = new FRTextBlock(Application->GetDefaultFontSet());
	TextBlock->SetText(StartupText);
	ViewRange.ItemCount = TextBlock->GetLineCount();
}

void FUITextBlock::OnAreaChanged()
{
	ViewRange.RangeItemCount = GetRenderingSize().Y / TextBlock->GetLineHeight();
}

void FUITextBlock::OnRendering()
{
	uint ViewCount = min(ViewRange.RangeItemCount, ViewRange.ItemCount - ViewRange.TopItemIndex);
	TextBlock->SetLineOffset(ViewRange.TopItemIndex);
	TextBlock->SetRenderingEndLine(ViewRange.TopItemIndex + ViewCount);
	TextBlock->Render(Renderer, int2(0), FontColor);
}

void FUITextBlock::SetText(wstring Text)
{ 
	if (TextBlock)
		TextBlock->SetText(Text);
	else
		StartupText = Text; 
}

wstring FUITextBlock::GetText() const
{
	if (TextBlock)
		return TextBlock->GetText();
	else
		return L"";
}