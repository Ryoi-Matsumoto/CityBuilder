#include "UIScrollView.h"
#include "..\Primitive\UIScrollBar.h"
#include "..\Primitive\UIButton.h"

void FUIScrollView::OnInitialize()
{
	assert(Content);

	AddXDefine(SUILenght(1, true));
	AddXDefine(SUILenght(30));

	Content->SetXIndex(0);

	auto Grid = new FUIGrid();
	Grid->SetXIndex(1);
	Grid->AddYDefine(SUILenght(25));
	Grid->AddYDefine(SUILenght(1, true));
	Grid->AddYDefine(SUILenght(25));
	Grid->SetMargin(SUIMargin(0, 8));
	Grid->SetBackColor(EUIColorType::ScrollBack);
	{
		SUIActiveColor ButtonColor(EUIColorType::Scroll, EUIColorType::ScrollOnMouse, EUIColorType::ScrollOnClick);
		
		ScrollBar = new FUIScrollBar();
		ScrollBar->SetYIndex(1);
		ScrollBar->SetTargetRange(Range);
		ScrollBar->SetBindControl(this);
		ScrollBar->SetBackColor(EUIColorType::ScrollBack);

		auto UpButton = new FUITriangle();
		UpButton->SetYIndex(0);
		UpButton->SetUpside(true);
		UpButton->SetActiveColor(ButtonColor);
		UpButton->SetEventOnClicked([=]() { ScrollBar->Scroll(-1); });
		UpButton->SetMargin(SUIMargin(8, 0));
		UpButton->SetBackColor(EUIColorType::ScrollBack);

		auto DownButton = new FUITriangle();
		DownButton->SetYIndex(2);
		DownButton->SetUpside(false);
		DownButton->SetActiveColor(ButtonColor);
		DownButton->SetEventOnClicked([=]() { ScrollBar->Scroll(1); });
		DownButton->SetMargin(SUIMargin(8, 0));
		DownButton->SetBackColor(EUIColorType::ScrollBack);

		Grid->AddChild(UpButton);
		Grid->AddChild(ScrollBar);
		Grid->AddChild(DownButton);
	}

	AddChild(Content);
	AddChild(Grid);
}

void FUIScrollView::OnMouseWheel(int Delta, int2 MousePosition)
{
	ScrollBar->Scroll(Delta * -2);
}