#include "../../Public/Control/UIDockingTab.h"
#include "../../Public/Control/UIButton.h"
#include "../../Public/Control/UITextBlock.h"
#include "../../Public/Control/Container/UIStackPanel.h"
#include "UIColor.h"

class FUIDockingItem : public FUIButton
{
private:
	void OnInitialize(FUIApplication* Application) override
	{
		UIBase
		.ActiveColor(SUIActiveColor(ColorMain, ColorScrollOnMouse, ColorSelection))
		[		
			UINew(FUIHorizontalStackPanel)
			.Margin(3)
			[
				UINew(FUITextButton)
				.Text(_Title)
				.OnClicked([this]() { _OnOpened(this); })

				+ UINew(FUITextButton)
				.Text(L"✕")
				.OnClicked([this]() { _OnClosed(this); })
			]
		];
	}

public:
	UIAttribute(TUIEvent<FUIDockingItem*>, OnOpened)
	UIAttribute(TUIEvent<FUIDockingItem*>, OnClosed)
	UIAttribute(wstring, Title)
	UIAttribute(FUIControl*, Content)
};

class FUIFrame : public FUIControlContainer
{
private:
	void OnAreaChanged()
	{
		if (Contents.size() > 0)
		{
			Contents[0]->SetArea(GetArea());
		}
	}
};

FUIDokingTab::FUIDokingTab()
{
}

void FUIDokingTab::OnInitialize(FUIApplication* Application)
{
	UIBase
	.YDefines({SUILenght(), SUILenght(2), SUILenght(1, true)})
	[
		UINewNamed(Header, FUIHorizontalStackPanel)	

		+ UINew(FUIControl)
		.BackColor(ColorSelection)

		+ (FUIControl&&)UINewNamed(Frame, FUIFrame)
	];
}

void FUIDokingTab::AddTabItem(FUIControl* Control, wstring Title)
{
	auto TabItem = new FUIDockingItem();//UINew(FUIDockingItem);
//	.OnOpened([this](FUIDockingItem* Item) { Frame->SetContent(Item->_Content); });

	Header->AddContent(TabItem);
}