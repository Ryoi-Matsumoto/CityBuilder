#pragma once
#include "UITextBlock.h"
#include "UIScrollBar.h"

// BindControl : スクロールに影響するコントロールを包括したコントロール

class FUITextBox : public FUITextBlock
{
private:
	vector<intrect> SelectAreaRectangles;
	uint TopIndexOnCreateSelectAreaRectangles;

	SRTextPlace Cursor;
	SRTextPlace CursorSelectFrom;
	SRTextPlace DragTo;

	bool IsTextSelected;
	bool IsSelectAreaDecided;

	bool IsCursorVisible;
	bool IsCursorFlash;

	bool IsMultiLine;

	bool IsDragging;
	bool IsDragOk;

private:
	void UpdateSelectArea();
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
	void OnDoubleClick(int2 MousePosition) override;
	void OnTimer() override;

public:
	FUITextBox();

	uint GetLineCount() const { return TextBlock->GetLines().size(); }
	void SetIsMultiLine(bool InIsMultiLine) { IsMultiLine = InIsMultiLine; }
};