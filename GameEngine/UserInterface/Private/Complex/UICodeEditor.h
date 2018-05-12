#pragma once
#include "..\Container\UIScrollView.h"
#include "..\Primitive\UITextBox.h"

class FUICodeEditor : public FUIScrollView
{
private:
	FUITextBox* TextBox;

private:
	void OnInitialize() override;

public:
	FUICodeEditor();

	FUITextBox* GetTextBox() const { return TextBox; }
};