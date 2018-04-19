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
			Renderer->DrawRectangle(NewRectangle, float4(0.1, 0.3, 1, 0.5));
		}
	}

	if (IsCursorVisible)
	{
		auto RenderingCursor = Cursor;
		if (IsDragging && IsDragOk)
			RenderingCursor = DragTo;
		auto Area = intrect(TextBlock->GetLetterPosition(RenderingCursor), int2(3, TextBlock->GetLineHeight()));
		Renderer->DrawRectangle(Area, float4(1));
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
			Cursor.Letter = TextBlock->GetLetterCount(Cursor.Line) - 1;
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
	UpdateAndCursorFlash();
}

void FUITextBox::OnMouseDown(EMouseButton Button, int2 MousePosition)
{
	// カーソルが選択範囲内のとき、ドラッグを開始する
	if (IsTextSelected && IsSelectAreaDecided)
		IsDragging = GetIsCursorInSelectArea(SelectAreaRectangles, MousePosition);

	// ドラッグしていないとき、カーソルを移動する
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
		if (IsDragging) // ドラッグ先を設定する
		{
			DragTo = TextBlock->GetTextPlace(Position);
			IsDragOk = !SRTextPlaceArea(Cursor, CursorSelectFrom).IsRange(DragTo);
			ChangeCursor(IsDragOk ? IDC_UPARROW : IDC_NO);
		}
		else // テキストを選択する
		{
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

	// OnTimerにおける描画以外では常にカーソルを表示させる
	IsCursorVisible = true;
}

void FUITextBox::UpdateSelectArea()
{
	TopIndexOnCreateSelectAreaRectangles = ViewRange.TopItemIndex;

	// 文字を入力しながら、選択範囲を削除するときに発生する範囲外エラーを修正
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

	int ViewableLineCount = GetRenderingSize().Y / TextBlock->GetLineHeight() - 1;
	int ViewingLineCount = Cursor.Line - TextBlock->GetLineOffset();
	if (ViewingLineCount > ViewableLineCount)
		ViewRange.TopItemIndex = TextBlock->GetLineOffset() + ViewingLineCount - ViewableLineCount;
	if (ViewingLineCount < 0)
		ViewRange.TopItemIndex = TextBlock->GetLineOffset() + ViewingLineCount;

	ViewRange.ItemCount = GetLineCount();

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