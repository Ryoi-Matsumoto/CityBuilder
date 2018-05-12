#include "..\Public\RTextBlock.h"

FRTextBlock::FRTextBlock(FRFontSet* FontSet)
	: FRTextContainer(FontSet), EndIndex(-1)
{
}

void FRTextBlock::Render(FR2DRenderer* Renderer, int2 Position, float4 Color, float2 Scale) const
{
	int2 Offset = 0;
	uint Count = EndIndex >= 0 ? FMath::Min(EndIndex, (uint)GetLines().size()) : GetLines().size();

	for (uint i = GetLineOffset(); i < Count; i++)
	{
		for (auto Letter : GetLines()[i])
		{
			Letter->Render(Renderer, Offset + Position, Color, Scale);
			Offset.X += Letter->GetWidth() * Scale.X;
		}

		Offset.X = 0;
		Offset.Y += GetLineHeight() * Scale.Y;
	}
}

uint FRTextBlock::GetWidth() const
{
	uint MaxWidth = 0;

	for (auto Line : GetLines())
	{
		uint Width = 0;
		for (auto Letter : Line)
			Width += Letter->GetWidth();
		MaxWidth = FMath::Max(MaxWidth, Width);
	}

	return MaxWidth;
}

uint FRTextBlock::GetHeight() const
{
	return GetLineHeight() * GetLineCount();
}