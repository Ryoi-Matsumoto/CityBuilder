#include "../../../Public/Control/Container/UIScrollView.h"
#include "../../../Public/Control/UIButton.h"
#include "../../../Public/Control/Container/UIGrid.h"
#include "../UIColor.h"

// 全項目数...Control依存
// 表示項目インデックス...ScrollBar依存, Control依存
// 表示可能項目数...Area依存

//WinAPI→ScrollView OnAreaChanged = 全項目数
//Control→ScrollView 
//ScrollView→

class FUISimpleScrollBar : public FUIControl
{
public:
	uint AllItemCount;
	uint ViewItemCount;
	uint ViewStartIndex;
	float4 BarColor;
	intrect BarRect;
	int MouseDownPositionY;
	uint MouseDownBeginIndex;
	
private:
	void SetViewStartIndex(int Index)
	{
		ViewStartIndex = FMath::Max(0, FMath::Min(AllItemCount - ViewItemCount, Index));
		_OnScrolled(ViewStartIndex);
	}

public:
	void Scroll(int Delta)
	{
		SetViewStartIndex(ViewStartIndex + Delta);
		Update();
	}

	void OnRendering() override
	{
		if (ViewItemCount == 0)return;
		if (ViewStartIndex == 0)return;

		int2 BarSize = int2(Area.Size.X, ViewItemCount / AllItemCount * Area.Size.Y);
		int2 BarPosition = int2(Area.Position.X, ViewStartIndex / AllItemCount * Area.Size.Y);
		BarRect = intrect(BarPosition, BarSize);
		Renderer->DrawRectangle(BarRect, BarColor);
	}

	void OnMouseDown(EMouseButton Button, int2 MousePosition) override
	{
		if (Button == EMouseButton::Right)
		{
			if (BarRect.IsRange(MousePosition))
			{
				MouseDownPositionY = MousePosition.Y;
				MouseDownBeginIndex = ViewStartIndex;
			}
			else
			{
				float Standard = GetArea().Position.Y + GetArea().Size.Y / 2.0f;
				if (MousePosition.Y > Standard)
				{
					ViewStartIndex = FMath::Max(0, ViewStartIndex - ViewItemCount);
				}
				else
				{
					ViewStartIndex = FMath::Min(AllItemCount - ViewItemCount, ViewStartIndex + ViewItemCount);
				}
				Update();
			}
		}
	};

	void OnMouseUp(EMouseButton Button, int2 MousePosition) override
	{
		if (Button == EMouseButton::Right)
		{
			MouseDownBeginIndex = 0;
		}
	};

	void OnMouseMove(int2 MousePosition) override
	{
		if (MouseDownPositionY != 0)
		{
			int MouseDelta = MousePosition.Y - MouseDownPositionY;
			int IndexDelta = MouseDelta / (Area.Size.Y / AllItemCount);
			SetViewStartIndex(MouseDownBeginIndex + IndexDelta);
			Update();
		}
	};

	UIAttribute(TUIEvent<uint>, OnScrolled)
};

class FUIScrollBar : public FUIGrid
{
private:
	FUISimpleScrollBar* ScrollBar;

public:	
	FUIScrollBar::FUIScrollBar()
	{
		_BackColor = ColorMain;

		UIBase
		.YDefines({ SUILenght(), SUILenght(1, true), SUILenght() })
		[
			UINew(FUITextButton)
			.Text(L"▲")
			.OnClicked([this]() { ScrollBar->Scroll(1); })
			.YIndex(0)

			+ UINewNamed(ScrollBar, FUISimpleScrollBar)
			.OnScrolled([this](int BeginIndex) { _OnScrolled(BeginIndex); })
			.YIndex(1)

			+ UINew(FUITextButton)
			.Text(L"▼")
			.OnClicked([this]() { ScrollBar->Scroll(-1); })
			.YIndex(2)
		];
	}

	void FUIScrollBar::SetItemCount(uint ItemCount)
	{
		ScrollBar->AllItemCount = ItemCount;
		ScrollBar->ViewStartIndex = FMath::Min(ScrollBar->ViewStartIndex, ItemCount);
		ScrollBar->Update();
	}

	void FUIScrollBar::SetViewRange(uint Range)
	{
		ScrollBar->ViewItemCount = Range;
		ScrollBar->Update();
	}

	uint FUIScrollBar::GetViewRange()
	{
		return ScrollBar->ViewItemCount;
	}

	UIAttribute(TUIEvent<uint>, OnScrolled)
};

FUIScrollView::FUIScrollView()
	: ScrollBar(new FUIScrollBar())
{
}

void FUIScrollView::OnInitialize(FUIApplication* Application)
{
	auto Event = [this](int Index) 
	{		
		_ScrollableControl->ViewStartIndex = Index;
		_ScrollableControl->OnScrolled();
		_ScrolledControl->Update();
	};
	ScrollBar->OnScrolled(Event);
	_ScrollableControl->UpdateScrollBar = [this]()
	{
		ScrollBar->SetItemCount(_ScrollableControl->AllItemCount);
		ScrollBar->Update();
	};

	Contents.emplace_back(_ScrolledControl);
	Contents.emplace_back(ScrollBar);
}

void FUIScrollView::OnAreaChanged()
{
	ScrollBar->SetArea(intrect(Area.Position + Area.Size - int2(25, 0), int2(25, Area.Size.Y)));
	_ScrolledControl->SetArea(intrect(Area.Position, Area.Size - int2(25, 0)));
}