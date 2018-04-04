#pragma once
#include "UIImage.h"
#include "Container\UIScrollView.h"
#include "Rendering\Public\RTextBlock.h"

class FUITextBlock : public FUIImage, public FUIScrollable
{
protected:
	FRTextBlock* TextBlock;
	float4 Color;

protected:
	void OnInitialize(FUIApplication* Application) override;
	void OnScrolled() override;

public:
	FUITextBlock();

	UIAttribute(wstring, Text)
};

class FUITextBox : public FUITextBlock
{
private:	
	SRTextPlace Cursor;
	SRTextPlace CursorSelectFrom;
	bool IsMousePressed;
	bool IsTextSelected;
	bool IsCursorVisible;

private:
	void UpdateAndCursorFlash();
	void DeleteSelectedArea();

private:
	void OnRendering() override;
	void OnGetFocus() override;
	void OnLostFocus() override;
	void OnKeyDown(wchar_t Key) override;
	void OnCharInputed(wchar_t Char) override;
	void OnMouseDown(EMouseButton Button, int2 MousePosition) override;
	void OnMouseUp(EMouseButton Button, int2 MousePosition) override;
	void OnMouseMove(int2 MousePosition) override;
	void OnMouseLeave(int2 MousePosition) override;
	//void OnMouseWheel(int Delta, int2 MousePosition) override;
	void OnTimer() override;

public:
	FUITextBox();
	void SetLineOffset(uint StartLineIndex);
	uint GetLineCount() const { return TextBlock->GetLines().size(); }
	
	UIAttribute(bool, IsMultiLine)
	UIAttribute(TUIEvent<>, OnTextChanged)
};

class FUITextEditor : public FUIScrollView
{
private:
	FUITextBox* TextBox;
	
public:
	FUITextEditor();
};