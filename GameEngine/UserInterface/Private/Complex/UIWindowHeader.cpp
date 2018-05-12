#include "UIWindowHeader.h"
#include "..\Primitive\UIButton.h"
#include "..\Container\UIStackPanel.h"

class FUITitleBar : public FUITextBlock
{
private:
	bool GetIsClientArea() const override final { return false; }
};

void FUIWindowHeader::OnInitialize()
{
	AddXDefine(SUILenght(1, true));
	AddXDefine(SUILenght());

	auto TitleBar = new FUITitleBar();
	TitleBar->SetText(GetWindow()->GetTitle());
	TitleBar->SetXIndex(0);

	auto StackPanel = new FUIHorizontalStackPanel();
	{
		SUIMargin ButtonMargin(10);

		auto MinimizeButton = new FUITextButton();
		MinimizeButton->SetText(L"_");//🗕
		MinimizeButton->SetMargin(ButtonMargin);

		auto MaximizeButton = new FUITextButton();
		MaximizeButton->SetText(L"□");//🗖🗗
		MaximizeButton->SetMargin(ButtonMargin);

		auto CloseButton = new FUITextButton();
		CloseButton->SetText(L"✕");//🗙🗔
		CloseButton->SetMargin(ButtonMargin); 

		StackPanel->AddChild(MinimizeButton);
		StackPanel->AddChild(MaximizeButton);
		StackPanel->AddChild(CloseButton);
	}
	StackPanel->SetXIndex(1);

	AddChild(TitleBar);
	AddChild(StackPanel);
}