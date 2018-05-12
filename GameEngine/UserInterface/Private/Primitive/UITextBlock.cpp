#include "UITextBlock.h"

FUITextBlock::FUITextBlock()
	: StartupText(L"TextBlock")
	, TextBlock(nullptr)
	, FontColor(float4(1))
{
}

void FUITextBlock::OnInitialize()
{
	TextBlock = new FRTextBlock(GetApplication()->GetDefaultFontSet());
	TextBlock->SetText(StartupText);
	ViewRange.ItemCount = TextBlock->GetLineCount();
}

void FUITextBlock::OnAreaChanged()
{
	ViewRange.RangeItemCount = GetRenderingSize().Y / TextBlock->GetLineHeight();
}

void FUITextBlock::OnRendering()
{
	auto Size = GetRenderingSize();
	//int2 Position = 0;
	//if(IsRenderingInCenter)
	//	Position = int2(Size.X / 2.0f - TextBlock->GetWidth(), Size.Y / 2.0f - TextBlock->GetHeight());
	uint ViewCount = min(ViewRange.RangeItemCount, ViewRange.ItemCount - ViewRange.TopItemIndex);
	TextBlock->SetLineOffset(ViewRange.TopItemIndex);
	TextBlock->SetRenderingEndLine(ViewRange.TopItemIndex + ViewCount);
	TextBlock->Render(GetRenderer(), 0, FontColor);
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