#include "UITextBox.h"

enum class ELetterKind
{
	Special, Symbol, Identifier, Other
};

ELetterKind GetLetterKind(char Letter)
{
	// http://digital.ni.com/public.nsf/allkb/4AC046450185922B86256F18004A03CF
	if (Letter <= 32)
		return ELetterKind::Special;
	else if (Letter <= 47)
		return ELetterKind::Symbol;
	else if (Letter <= 57)
		return ELetterKind::Identifier;
	else if (Letter <= 64)
		return ELetterKind::Symbol;
	else if (Letter <= 90)
		return ELetterKind::Identifier;
	else if (Letter <= 96)
		return ELetterKind::Symbol;
	else if (Letter <= 122)
		return ELetterKind::Identifier;
	else if (Letter <= 126)
		return ELetterKind::Symbol;
	else
		return ELetterKind::Other;
}

bool GetIsCursorInSelectArea(vector<intrect> const& SelectAreaRectangles, int2 MousePosition)
{
	for (auto Rectangle : SelectAreaRectangles)
	{
		if (Rectangle.IsInRange(MousePosition))
			return true;
	}
	return false;
}

FUITextBox::FUITextBox()
	: Cursor(0, 0)
	, CursorSelectFrom(0, 0)
	, IsTextSelected(false)
	, IsCursorVisible(true)
	, IsMultiLine(true)
	, IsDragging(false)
	, IsSelectAreaDecided(false)
	, OnTextChanged(nullptr)
{
}

vector<intrect> GetTextAreaRectangles(FRTextBlock* TextBlock, SRTextPlaceArea Area)
{
	vector<intrect> Result;

	if (Area.FromPlace.Line - Area.ToPlace.Line == 0)
	{
		uint Width = TextBlock->GetLineBetweenWidth(Area.FromPlace, Area.ToPlace.Letter - Area.FromPlace.Letter);
		int2 Position = TextBlock->GetLetterPosition(Area.FromPlace);
		int2 Size = int2(Width, TextBlock->GetLineHeight());
		Result.push_back(intrect(Position, Size));
	}
	else
	{
		uint MinWidth = TextBlock->GetFontSize() / 2.0;

		uint FirstWidth = TextBlock->GetLineLatterWidth(Area.FromPlace);
		int2 FirstPosition = TextBlock->GetLetterPosition(Area.FromPlace);
		int2 FirstSize = int2(FirstWidth, TextBlock->GetLineHeight());
		Result.push_back(intrect(FirstPosition, FirstSize));

		uint LastWidth = TextBlock->GetLineFormerWidth(Area.ToPlace);
		int2 LastPosition = TextBlock->GetLetterPosition(SRTextPlace(Area.ToPlace.Line, 0));
		int2 LastSize = int2(max(MinWidth, LastWidth), TextBlock->GetLineHeight());
		Result.push_back(intrect(LastPosition, LastSize));

		for (int i = Area.FromPlace.Line + 1; i < Area.ToPlace.Line; i++)
		{
			uint Width = TextBlock->GetLineWidth(i);
			int2 Position = TextBlock->GetLetterPosition(SRTextPlace(i, 0));
			int2 Size = int2(max(MinWidth, Width), TextBlock->GetLineHeight());
			Result.push_back(intrect(Position, Size));
		}
	}
	return Result;
}

void FUITextBox::OnRendering()
{
	FUITextBlock::OnRendering();

	if (IsTextSelected)
	{
		uint LineCount = TopIndexOnCreateSelectAreaRectangles - ViewRange.TopItemIndex;
		int2 Offset = int2(0, TextBlock->GetLineHeight() * LineCount);

		for (auto Rectangle : SelectAreaRectangles)
		{
			intrect NewRectangle = Rectangle;
			NewRectangle.Position += Offset;
			GetRenderer()->DrawRectangle(NewRectangle, float4(0.1, 0.3, 1, 0.5));
		}
	}

	if (IsCursorVisible)
	{
		auto RenderingCursor = Cursor;
		if (IsDragging && IsDragOk)
			RenderingCursor = DragTo;
		auto Area = intrect(TextBlock->GetLetterPosition(RenderingCursor), int2(3, TextBlock->GetLineHeight()));
		GetRenderer()->DrawRectangle(Area, float4(1));
	}

	RegisterTimer(GetCaretBlinkTime());
}

void FUITextBox::OnKeyDown(wchar_t Key)
{
	switch (Key)
	{
	case VK_UP:
		if (IsMultiLine)
		{
			if (IsTextSelected)
			{
				IsTextSelected = false;
				Cursor = SRTextPlace::Near(CursorSelectFrom, Cursor);
			}
			if (Cursor.Line > 0)
			{
				int2 PrevPosition = TextBlock->GetLetterPosition(Cursor);
				Cursor.Line--;
				Cursor.Letter = TextBlock->GetLetterNumber(Cursor.Line, PrevPosition.X);
			}
			UpdateAndCursorFlash();
		}
		break;
	case VK_DOWN:
		if (IsMultiLine)
		{
			if (IsTextSelected)
			{
				IsTextSelected = false;
				Cursor = SRTextPlace::Far(CursorSelectFrom, Cursor);
			}
			if (Cursor.Line < TextBlock->GetLineCount() - 1)
			{
				int2 PrevPosition = TextBlock->GetLetterPosition(Cursor);
				Cursor.Line++;
				Cursor.Letter = TextBlock->GetLetterNumber(Cursor.Line, PrevPosition.X);
			}
			UpdateAndCursorFlash();
		}
		break;
	case VK_RIGHT:
		if (IsTextSelected)
		{
			IsTextSelected = false;
			Cursor = SRTextPlace::Far(CursorSelectFrom, Cursor);
		}
		else if (Cursor.Letter < TextBlock->GetLetterCount(Cursor.Line))
		{
			Cursor.Letter++;
		}
		else if (Cursor.Line < TextBlock->GetLineCount() - 1)
		{
			Cursor.Line++;
			Cursor.Letter = 0;
		}

		UpdateAndCursorFlash();
		break;
	case VK_LEFT:
		if (IsTextSelected)
		{
			IsTextSelected = false;
			Cursor = SRTextPlace::Near(CursorSelectFrom, Cursor);
		}
		else if (Cursor.Letter > 0)
		{
			Cursor.Letter--;
		}
		else if (Cursor.Line > 0)
		{
			Cursor.Line--;
			Cursor.Letter = FMath::Max(TextBlock->GetLetterCount(Cursor.Line), 1) - 1;
		}
		UpdateAndCursorFlash();
		break;
	case VK_RETURN:
		if (IsMultiLine)
		{
			if (IsTextSelected)
				DeleteSelectedArea();
			Cursor = TextBlock->InsertReturn(Cursor);
			UpdateAndCursorFlash();
		}
		break;
	case VK_BACK:
		if (IsTextSelected)
		{
			DeleteSelectedArea();
		}
		else
		{
			if (Cursor.Letter == 0)
			{
				if (IsMultiLine && Cursor.Line > 0)
					Cursor = TextBlock->LinkPreviusLine(Cursor.Line);
			}
			else
			{
				Cursor = TextBlock->DeleteLetter(Cursor);
			}
		}
		UpdateAndCursorFlash();
		break;
	}
}

void FUITextBox::OnGetFocus()
{
	IsCursorVisible = true;
	UpdateAndCursorFlash();
}

void FUITextBox::OnLostFocus()
{
	IsCursorVisible = false;
	UpdateAndCursorFlash();
}

void FUITextBox::OnCharInputed(wchar_t Char)
{
	if (Char == L'\b' || Char == L'\r' || Char == L'\n')
		return;

	if (IsTextSelected)
		DeleteSelectedArea();
	Cursor = TextBlock->InsertLetter(Cursor, Char);
	if (OnTextChanged)
		OnTextChanged(TextBlock->GetText());
	UpdateAndCursorFlash();
}

void FUITextBox::OnMouseDown(EMouseButton Button, int2 MousePosition)
{
	// when the cursor is in the range of the selected area, start dragging 
	if (IsTextSelected && IsSelectAreaDecided)
		IsDragging = GetIsCursorInSelectArea(SelectAreaRectangles, MousePosition);

	// when not dragging, move the cursor
	if (!IsDragging)
	{
		IsTextSelected = false;
		CursorSelectFrom = Cursor = TextBlock->GetTextPlace(MousePosition);
		UpdateAndCursorFlash();
	}
}

void FUITextBox::OnMouseUp(EMouseButton Button, int2 MousePosition)
{
	if (IsDragging && IsDragOk)
	{
		TextBlock->DragAndDropText(SRTextPlaceArea(Cursor, CursorSelectFrom), DragTo);
		ChangeCursor(IDC_ARROW);
	}

	if (IsTextSelected)
	{
		if (IsSelectAreaDecided)
		{
			IsTextSelected = false;
			IsDragging = false;
			UpdateAndCursorFlash();
		}
		else
		{
			IsSelectAreaDecided = true;
		}
	}
}

void FUITextBox::OnMouseMove(int2 Position)
{
	ChangeCursor(IDC_IBEAM);

	if (GetMouseButtonDown(EMouseButton::Left))
	{	
		if (IsDragging)
		{
			// set the drag destination
			DragTo = TextBlock->GetTextPlace(Position);
			IsDragOk = !SRTextPlaceArea(Cursor, CursorSelectFrom).IsRange(DragTo);
			ChangeCursor(IsDragOk ? IDC_UPARROW : IDC_NO);
		}
		else
		{
			// set the drag text
			Cursor = TextBlock->GetTextPlace(Position);
			IsTextSelected = CursorSelectFrom.Line != Cursor.Line || CursorSelectFrom.Letter != Cursor.Letter;
			IsSelectAreaDecided = false;
			if (IsTextSelected)
				UpdateSelectArea();
		}
		UpdateAndCursorFlash();
	}
}

void FUITextBox::OnMouseLeave(int2 MousePosition)
{
	ChangeCursor(IDC_ARROW);
}

void FUITextBox::OnDoubleClick(int2 MousePosition)
{
	auto& Line = TextBlock->GetLines()[Cursor.Line];

	if (Cursor.Letter >= Line.size())
		return;

	// spread the selected area
	auto TargetKind = GetLetterKind(Line[Cursor.Letter]->GetLetter());

	uint SelectStart = Cursor.Letter;
	for (; SelectStart > 0; SelectStart--)
	{
		auto Kind = GetLetterKind(Line[SelectStart]->GetLetter());
		if (TargetKind != Kind)
		{
			SelectStart++;
			break;
		}
	}

	uint SelectEnd = Cursor.Letter;
	for (; SelectEnd < Line.size(); SelectEnd++)
	{
		auto Kind = GetLetterKind(Line[SelectEnd]->GetLetter());
		if (TargetKind != Kind)
			break;
	}

	// when selecting text
	if (Cursor.Letter > SelectStart || Cursor.Letter < SelectEnd)
	{
		CursorSelectFrom.Line = Cursor.Line;
		CursorSelectFrom.Letter = SelectStart;
		Cursor.Letter = SelectEnd;
		IsTextSelected = true;
		IsSelectAreaDecided = false;
		UpdateSelectArea();
		UpdateAndCursorFlash();
	}
}

void FUITextBox::OnTimer()
{
	IsCursorFlash = !IsCursorFlash;
	IsCursorVisible = IsCursorFlash;
	Update();

	// except when rendering in OnTimer, always show the cursor
	IsCursorVisible = true;
}

void FUITextBox::UpdateSelectArea()
{
	TopIndexOnCreateSelectAreaRectangles = ViewRange.TopItemIndex;

	// correct out of range error that occurs when deleting selected text while typing
	uint LineCount = TextBlock->GetLineCount();
	CursorSelectFrom.Line = min(CursorSelectFrom.Line, LineCount - 1);
	uint LetterCount = TextBlock->GetLetterCount(CursorSelectFrom.Line);
	CursorSelectFrom.Letter = min(CursorSelectFrom.Letter, LetterCount);

	auto Area = SRTextPlaceArea(CursorSelectFrom, Cursor);
	SelectAreaRectangles = GetTextAreaRectangles(TextBlock, Area);
}

void FUITextBox::UpdateAndCursorFlash()
{
	IsCursorVisible = true;

	// update the view range
	int ViewableLineCount = GetRenderingSize().Y / TextBlock->GetLineHeight() - 1;
	int ViewingLineCount = Cursor.Line - TextBlock->GetLineOffset();
	if (ViewingLineCount > ViewableLineCount)
		ViewRange.TopItemIndex = TextBlock->GetLineOffset() + ViewingLineCount - ViewableLineCount;
	if (ViewingLineCount < 0)
		ViewRange.TopItemIndex = TextBlock->GetLineOffset() + ViewingLineCount;

	ViewRange.ItemCount = GetLineCount();

	// render
	if (GetBindControl())
		GetBindControl()->Update();
	else
		Update();
}

void FUITextBox::DeleteSelectedArea()
{
	IsTextSelected = false;
	auto Area = SRTextPlaceArea(CursorSelectFrom, Cursor);
	Cursor = TextBlock->DeleteLettersBetween(Area);
}