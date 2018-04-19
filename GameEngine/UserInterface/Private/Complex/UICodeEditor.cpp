#include "UICodeEditor.h"
#include "..\Container\UIScrollView.h"
#include "..\Primitive\UILineNumber.h"
#include "..\Primitive\UITextBox.h"

void FUICodeEditor::OnInitialize(FUIApplication* Application)
{
	FUITextBox* TextBox;

	auto Grid = new FUIGrid();
	Grid->AddXDefine(SUILenght());
	Grid->AddXDefine(SUILenght(1, true));
	{
		TextBox = new FUITextBox();
		TextBox->SetXIndex(1);
		TextBox->SetBindControl(this);

		auto LineNumber = new FUILineNumber();
		LineNumber->SetXIndex(0);
		LineNumber->SetViewRange(TextBox->GetViewRange());
		LineNumber->SetMargin(SUIMargin(0, 0, 0, 20));

		Grid->AddChild(LineNumber);
		Grid->AddChild(TextBox);
	}

	SetContent(Grid);
	SetViewRange(TextBox->GetViewRange());

	FUIScrollView::OnInitialize(Application);
}