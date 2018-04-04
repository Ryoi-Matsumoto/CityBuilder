#include "../../../Public/Control/Container/UIStackPanel.h"

FUIVerticalStackPanel::FUIVerticalStackPanel()
{
}

void FUIVerticalStackPanel::OnAreaChanged()
{
	int Offset = 0;
	for (uint i = 0; i < Contents.size(); i++)
	{
		intrect ContentArea(Area);
		ContentArea.Position.Y += Offset;
		ContentArea.Size.Y = Contents[i]->GetHeight();
		Offset += ContentArea.Size.Y;
		Contents[i]->SetArea(ContentArea);
	}
}

uint FUIVerticalStackPanel::GetWidth() const
{
	uint MaxWidth = 0;
	for (uint i = 0; i < Contents.size(); i++)
	{
		MaxWidth = FMath::Max(MaxWidth, Contents[i]->GetWidth());
	}
	return MaxWidth;
}

uint FUIVerticalStackPanel::GetHeight() const
{
	uint Height = 0;
	for (uint i = 0; i < Contents.size(); i++)
	{
		Height += Contents[i]->GetHeight();
	}
	return Height;
}

FUIHorizontalStackPanel::FUIHorizontalStackPanel()
{
}

void FUIHorizontalStackPanel::OnAreaChanged()
{
	int Offset = 0;
	for (uint i = 0; i < Contents.size(); i++)
	{
		Area.Position.X += Offset;
		Area.Size.X = Contents[i]->GetWidth();
		Offset += Area.Size.X;
		Contents[i]->SetArea(Area);
	}
}

uint FUIHorizontalStackPanel::GetWidth() const
{
	uint Width = 0;
	for (uint i = 0; i < Contents.size(); i++)
	{
		Width += Contents[i]->GetWidth();
	}
	return Width;
}

uint FUIHorizontalStackPanel::GetHeight() const
{
	uint MaxHeight = 0;
	for (uint i = 0; i < Contents.size(); i++)
	{
		MaxHeight = FMath::Max(MaxHeight, Contents[i]->GetHeight());
	}
	return MaxHeight;
}