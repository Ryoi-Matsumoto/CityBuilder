#include "..\Public\RTextBlock.h"

FRTextBlock::FRTextBlock(FRFontSet* FontSet)
	: FRTextContainer(FontSet)
{
}

void FRTextBlock::Render(FR2DRenderer* Renderer, int2 Position, float4 Color) const
{
	int2 Offset = int2(0, GetLineHeight());
	for (uint i = GetLineOffset(); i < GetLines().size(); i++)
	{
		for (auto Letter : GetLines()[i])
		{
			Letter->Render(Renderer, Offset + Position, Color);
			Offset.X += Letter->GetWidth();
		}

		Offset.X = 0;
		Offset.Y += GetLineHeight();
	}
}

uint FRTextBlock::GetWidth() const
{
	uint MaxWidth = 0;
	for (auto Line : GetLines())
	{
		uint Width = 0;
		for (auto Letter : Line)Width += Letter->GetWidth();
		MaxWidth = FMath::Max(MaxWidth, Width);
	}
	return MaxWidth;
}

uint FRTextBlock::GetHeight() const
{
	return GetLineHeight() * GetLineCount();
}