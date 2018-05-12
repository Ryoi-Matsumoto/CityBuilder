#include "UIUpperTab.h"
#include "..\Primitive\UITextBlock.h"
#include "..\Primitive\UIButton.h"
#include "..\Container\UIStackPanel.h"
#include "UISplitter.h"

class FUITabItemHeader : public FUIHorizontalStackPanel
{
private:
	FUIUpperTab* ParentTab;
	SUIDockingItem* Item;
	FUITextButton* CloseButton;
	bool IsSelected;

public:
	FUITabItemHeader(FUIUpperTab* ParentTab, SUIDockingItem* Item) 
		: ParentTab(ParentTab)
		, Item(Item)
		, CloseButton(new FUITextButton())
	{
		SetIsSelected(false);
	}

	void OnMouseMove(int2 MousePosition) override
	{
		if (!IsSelected)
		{
			SetBackColor(EUIColorType::TabOnCursor);
			Update();
		}

		if (!GetArea().IsInRange(MousePosition) && GetMouseButtonDown(EMouseButton::Left))
		{
			ResetMouseButtonDown();
			SetIsSelected(false);
			ParentTab->RemoveItem(this);
			ParentTab->DragItem(Item);
		}
	}

	void OnMouseLeave(int2 MousePosition) override
	{
		if (!IsSelected)
		{
			SetBackColor(EUIColorType::Base);
			Update();
		}
	}

	void OnMouseDown(EMouseButton Button, int2 MousePosition) override
	{
		ParentTab->SelectItem(this, Item->Control);
	}

	void OnInitialize() override
	{
		auto HeaderText = new FUITextBlock();
		HeaderText->SetBackColor(EUIColorType::None);
		HeaderText->SetMargin(5);
		HeaderText->SetText(Item->Title);

		CloseButton->SetBackColor(EUIColorType::None);
		CloseButton->SetActiveColor(SUIActiveColor(EUIColorType::TabOnCursor, EUIColorType::TabButtonOnCursor, EUIColorType::ButtonOnClick));
		CloseButton->SetText(L"✕");

		AddChild(HeaderText);
		AddChild(CloseButton);

		SetMargin(5);
	}

	void SetIsSelected(bool InIsSelected)
	{
		IsSelected = InIsSelected;
		CloseButton->SetIsVisible(IsSelected);
		BackColor = IsSelected ? EUIColorType::Selection : EUIColorType::Base;
	}

	FUIControl* GetControl() const
	{
		return Item->Control;
	}
};

FUIUpperTab::FUIUpperTab()
	: HeaderStackPanel(new FUIHorizontalStackPanel())
	, Frame(new FUIControl())
	, SelectedHeader(nullptr)
	, PreviusSelectedHeader(nullptr)
{
}

void FUIUpperTab::OnInitialize()
{
	AddYDefine(SUILenght());
	AddYDefine(SUILenght(3));
	AddYDefine(SUILenght(1, true));
	{
		HeaderStackPanel->SetYIndex(0);
		Frame->SetYIndex(2);

		auto Space = new FUIControl();
		Space->SetBackColor(EUIColorType::Selection);
		Space->SetYIndex(1);

		AddChild(HeaderStackPanel);
		AddChild(Space);
		AddChild(Frame);
	}
}

void FUIUpperTab::AddItem(SUIDockingItem* Item)
{
	auto Header = new FUITabItemHeader(this, Item);
	HeaderStackPanel->AddChild(Header);
	SelectItem(Header, Item->Control);
}

void FUIUpperTab::SelectItem(FUITabItemHeader* Header, FUIControl* Control)
{
	if (SelectedHeader)
		SelectedHeader->SetIsSelected(false);

	if (GetIsInitialized() && !Control->GetIsInitialized())
		Control->Initialize(Application);

	Header->SetIsSelected(true);
	PreviusSelectedHeader = SelectedHeader;
	SelectedHeader = Header;
	Frame->SetOnlyChild(Control);

	if (GetIsInitialized())
		Update();
}

void FUIUpperTab::RemoveItem(class FUITabItemHeader* Header)
{
	HeaderStackPanel->RemoveChild(Header);

	if (PreviusSelectedHeader)
		Frame->SetOnlyChild(PreviusSelectedHeader->GetControl());
	else
		Frame->SetOnlyChild(nullptr);

	if (GetIsInitialized())
		Update();
}

void FUIUpperTab::DropItem(int2 MousePosition, SUIDockingItem* Item, bool HilightNotDrop)
{
	auto Area = GetRenderingArea();
	auto CenterArea = intrect(Area.Position + Area.Size / 3, Area.Size / 3);
	auto HilightColor = Application->GetColor(EUIColorType::Selection);

	// first tab
	if (CenterArea.IsInRange(MousePosition))
	{
		AddItem(Item);
	}
	// specified tab
	else if(MousePosition.Y <= Area.Position.Y + HeaderStackPanel->GetHeight())
	{
		for (auto& Header : HeaderStackPanel->GetChildren())
		{
			auto HeaderArea = Header->GetArea();
			if (Area.Position.Y <= HeaderArea.Position.X + HeaderArea.Size.X)
			{
				GetRenderer()->DrawRectangle(HeaderArea, HilightColor);
				GetRenderer()->DrawRectangle(Frame->GetArea(), HilightColor);
			}
		}
	}
	// upper
	else if (MousePosition.Y <= Area.Position.Y + Area.Size.Y / 3.f)
	{

	}
	// sides
	else if (MousePosition.Y <= Area.Position.Y + Area.Size.Y / 3.f * 2.f)
	{
		// left
		if (MousePosition.X <= Area.Position.X + Area.Size.X / 2.f)
		{
			auto NewTab = new FUIUpperTab();
			NewTab->AddItem(Item);
			//auto Splitter = new FUIHorizontalSplitter(NewTab, this);
		}
		// right
		else
		{
			auto NewTab = new FUIUpperTab();
			NewTab->AddItem(Item);
			//auto Splitter = new FUIHorizontalSplitter(this, NewTab);
		}
	}
	//lower
	else
	{

	}
}