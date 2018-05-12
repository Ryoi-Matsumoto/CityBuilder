#include "UILineNumber.h"

void FUILineNumber::OnInitialize()
{
	FontSize.Y = GetApplication()->GetDefaultFontSet()->GetFontType().Size;
	FontSize.X = FontSize.Y / 2;
	for (uint i = 0; i < 10; i++)
		NumberFonts[i] = GetApplication()->GetDefaultFontSet()->GetFontLetter('0' + i);
	Width = FontSize.X;
}

void FUILineNumber::OnRendering()
{
	uint ViewLineCount = min(Range->RangeItemCount, Range->ItemCount - Range->TopItemIndex) + 1;
	auto NumberColor = GetApplication()->GetColor(EUIColorType::LineNumber);
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
			NumberFont->Render(GetRenderer(), int2(XPosition, i * FontSize.Y), NumberColor);
		}
	}
}