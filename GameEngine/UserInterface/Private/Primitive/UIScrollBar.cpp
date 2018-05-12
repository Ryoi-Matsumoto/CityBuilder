#include "UIScrollBar.h"

FUIScrollBar::FUIScrollBar() 
	: TargetRange(nullptr)
	, BarColor(EUIColorType::Scroll) 
	, MouseDownBeginIndex(0)
	, MouseDownPositionY(0)
{
}

void FUIScrollBar::OnRendering()
{
	assert(TargetRange);
	
	if (TargetRange->RangeItemCount == 0 || TargetRange->ItemCount == 0)
		return;

	int2 Size = GetRenderingSize();
	uint ScrollableCount = TargetRange->ItemCount + TargetRange->RangeItemCount - 1;
	int2 BarSize = int2(Size.X, (float)TargetRange->RangeItemCount / ScrollableCount * Size.Y);
	int2 BarPosition = int2(0, (float)TargetRange->TopItemIndex / ScrollableCount * Size.Y);
	BarRectangle = intrect(BarPosition, BarSize);
	GetRenderer()->DrawRectangle(BarRectangle, GetApplication()->GetColor(BarColor));
}

void FUIScrollBar::OnMouseDown(EMouseButton Button, int2 MousePosition)
{
	if (Button == EMouseButton::Left)
	{
		if (BarRectangle.IsInRange(MousePosition))
		{
			BarColor = EUIColorType::ScrollOnClick;
			MouseDownPositionY = MousePosition.Y;
			MouseDownBeginIndex = TargetRange->TopItemIndex;
		}
		else
		{
			auto Area = GetRenderingArea();
			float Standard = Area.Position.Y + Area.Size.Y / 2.0f;
			if (MousePosition.Y > Standard)
				TargetRange->TopItemIndex = FMath::Max(0, TargetRange->TopItemIndex - TargetRange->RangeItemCount);
			else
				TargetRange->TopItemIndex = FMath::Min(TargetRange->ItemCount - TargetRange->RangeItemCount, TargetRange->TopItemIndex + TargetRange->RangeItemCount);
		}
		Update();
	}
}

void FUIScrollBar::OnMouseUp(EMouseButton Button, int2 MousePosition)
{
	if (Button == EMouseButton::Left)
	{
		BarColor = EUIColorType::Scroll;
		MouseDownBeginIndex = -1;
		Update();
	}
}

void FUIScrollBar::OnMouseMove(int2 MousePosition)
{
	if (MouseDownBeginIndex >= 0)
	{
		BarColor = EUIColorType::ScrollOnClick;
		uint ScrollableCount = TargetRange->ItemCount + TargetRange->RangeItemCount;
		int MouseDelta = MousePosition.Y - MouseDownPositionY;
		int IndexDelta = (float)MouseDelta / GetRenderingSize().Y * ScrollableCount;
		SetTopIndex(MouseDownBeginIndex + IndexDelta);
		Update();
	}
	else if (BarRectangle.IsInRange(MousePosition))
	{
		BarColor = EUIColorType::ScrollOnMouse;
		Update();
	}
}

void FUIScrollBar::OnMouseLeave(int2 MousePosition)
{
	BarColor = EUIColorType::Scroll;
	Update();
}

void FUIScrollBar::SetTopIndex(int Index)
{
	TargetRange->TopItemIndex = min(TargetRange->ItemCount - 1, max(Index, 0));

	if (GetBindControl())
		GetBindControl()->Update();
	else
		Update();
}

void FUIScrollBar::Scroll(int Delta)
{
	SetTopIndex(TargetRange->TopItemIndex + Delta);
}