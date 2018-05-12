#include "UISplitter.h"

void FUIHorizontalSplitter::OnInitialize()
{
	AddXDefine(SUILenght(1, true));
	AddXDefine(SUILenght(1, true));

	LeftArea->SetXIndex(0);
	RightArea->SetXIndex(1);

	AddChild(LeftArea);
	AddChild(RightArea);
}

void FUIVerticalSplitter::OnInitialize()
{
	AddYDefine(SUILenght(1, true));
	AddYDefine(SUILenght(1, true));
}