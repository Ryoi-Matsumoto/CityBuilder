#include "UILineNumber.h"
#include "..\Container\UIContainer.h"

void FUILineNumber::OnInitialize(FUIApplication* Application)
{
	FontSize.Y = Application->GetDefaultFontSet()->GetFontType().Size;
	FontSize.X = FontSize.Y / 2;
	for (uint i = 0; i < 10; i++)
		NumberFonts[i] = Application->GetDefaultFontSet()->GetFontLetter('0' + i);
	Width = FontSize.X;
}

void FUILineNumber::OnRendering()
{
	uint ViewLineCount = min(Range->RangeItemCount, Range->ItemCount - Range->TopItemIndex) + 1;
	auto NumberColor = GetWindow()->GetApplication()->GetColor(EUIColorType::LineNumber);
	uint OldWidth = Width;
	Width = ((uint)log10(Range->ItemCount) + 1) * FontSize.X;

	if (OldWidth != Width && GetParent())
	{
		GetParent()->UpdateArea();
		return;
	}

	for (uint i = 0; i < ViewLineCount; i++)
	{
		auto NumberString = to_string(Range->TopItemIndex + i);
		uint XPosition = Width;
		for (int j = NumberString.size() - 1; j >= 0; j--)
		{
			auto NumberFont = NumberFonts[NumberString[j] - '0'];
			XPosition -= NumberFont->GetWidth();
			NumberFont->Render(Renderer, int2(XPosition, i * FontSize.Y), NumberColor);
		}
	}
}