#include "UIStackPanel.h"

void FUIVerticalStackPanel::OnAreaChanged()
{
	int Offset = 0;
	for(auto& Child : Children)
	{
		intrect ContentArea(GetRenderingArea());
		ContentArea.Position.Y += Offset;
		ContentArea.Size.Y = Child->GetHeight();
		Offset += ContentArea.Size.Y;
		Child->SetArea(ContentArea);
	}
}

uint FUIVerticalStackPanel::GetActualHeight() const
{
	uint Height = 0;
	for (auto& Child : Children)
		Height += Child->GetHeight();
	return Height;
}

void FUIHorizontalStackPanel::OnAreaChanged()
{
	int Offset = 0;
	for (auto& Child : Children)
	{
		intrect ContentArea(GetRenderingArea());
		ContentArea.Position.X += Offset;
		ContentArea.Size.X = Child->GetWidth();
		Offset += ContentArea.Size.X;
		Child->SetArea(ContentArea);
	}
}

uint FUIHorizontalStackPanel::GetActualWidth() const
{
	uint Width = 0;
	for (auto& Child : Children)
		Width += Child->GetWidth();
	return Width;
}