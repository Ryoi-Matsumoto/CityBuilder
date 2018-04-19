#include "..\..\Public\Control\UIWindowControl.h"
#include "..\..\Public\Control\UIButton.h"
#include "..\..\Public\Control\UITextBlock.h"

FUIWindowControl::FUIWindowControl()
{
	UIBase
	.YDefines({SUILenght(50), SUILenght(1, true), SUILenght(150)})
	[
		UINew(FUITextBlock)
		.Text(L"卍")
		.YIndex(1)

		+ UINew(FUITextBlock)
		.Text(L"Window Title")
		.YIndex(1)

		+ UINew(FUIGrid)
		.XDefines({ SUILenght(1, true), SUILenght(50), SUILenght(50), SUILenght(50) })
		.YIndex(0)		
		[
			UINew(FUITextButton)
			.Text(L"_")

			+ UINew(FUITextButton)
			.Text(L"◻")

			+ UINew(FUITextButton)
			.Text(L"✕")
		]
	];
}

void FUIWindowControl::OnAreaChanged()
{
	GetWindow()->GetViewport()->Resize(Area.Size);
}