#include "UIStackPanel.h"

void FUIVerticalStackPanel::OnAreaChanged()
{
	int Offset = 0;
	for (uint i = 0; i < Children.size(); i++)
	{
		intrect ContentArea(GetArea());
		ContentArea.Position.Y += Offset;
		ContentArea.Size.Y = Children[i]->GetHeight();
		Offset += ContentArea.Size.Y;
		Children[i]->SetArea(ContentArea);
	}
}

uint FUIVerticalStackPanel::GetActualWidth() const
{
	uint MaxWidth = 0;
	for (uint i = 0; i < Children.size(); i++)
		MaxWidth = FMath::Max(MaxWidth, Children[i]->GetWidth());
	return MaxWidth;
}

uint FUIVerticalStackPanel::GetActualHeight() const
{
	uint Height = 0;
	for (uint i = 0; i < Children.size(); i++)
		Height += Children[i]->GetHeight();
	return Height;
}

void FUIHorizontalStackPanel::OnAreaChanged()
{
	int Offset = 0;
	for (uint i = 0; i < Children.size(); i++)
	{
		intrect ContentArea(GetArea());
		ContentArea.Position.X += Offset;
		ContentArea.Size.X = Children[i]->GetWidth();
		Offset += ContentArea.Size.X;
		Children[i]->SetArea(ContentArea);
	}
}

uint FUIHorizontalStackPanel::GetActualWidth() const
{
	uint Width = 0;
	for (uint i = 0; i < Children.size(); i++)
		Width += Children[i]->GetWidth();
	return Width;
}

uint FUIHorizontalStackPanel::GetActualHeight() const
{
	uint MaxHeight = 0;
	for (uint i = 0; i < Children.size(); i++)
		MaxHeight = FMath::Max(MaxHeight, Children[i]->GetHeight());
	return MaxHeight;
}