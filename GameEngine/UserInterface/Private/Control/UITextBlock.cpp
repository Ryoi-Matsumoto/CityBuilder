#include "..\..\Public\Control\UITextBlock.h"
#include "..\..\Public\Control\Container\UIGrid.h"
#include "UIColor.h"
#include <Windows.h>

FUITextBlock::FUITextBlock()
	: _Text(L"TextBlock"), Color(float4(1))
{
	_BackColor = ColorMain;
}

void FUITextBlock::OnInitialize(FUIApplication* Application)
{
	TextBlock = new FRTextBlock(Application->GetDefaultFontSet());
	TextBlock->SetText(_Text);
	_Image = TextBlock;
}

void FUITextBlock::OnScrolled()
{
	TextBlock->SetLineOffset(GetViewStartIndex());
}

FUITextBox::FUITextBox()
	: Cursor(0, 0)
	, CursorSelectFrom(0, 0)
	, IsMousePressed(false)
	, IsTextSelected(false)
	, IsCursorVisible(true)
	, _IsMultiLine(true)
{
}

void RenderTextArea(FRManager* Renderer, FRTextBlock* TextBlock, SRTextPlaceArea Area, float4 SelectionColor)
{
	if (Area.FromPlace.Line - Area.ToPlace.Line == 0)
	{
		uint Width = TextBlock->GetLineBetweenWidth(Area.FromPlace, Area.ToPlace.Letter - Area.FromPlace.Letter);
		Renderer->DrawRectangle
		(
			intrect
			(
				TextBlock->GetLetterPosition(Area.FromPlace),
				int2(Width, TextBlock->GetLineHeight())
			),
			SelectionColor
		);
	}
	else
	{
		uint FirstWidth = TextBlock->GetLineLatterWidth(Area.FromPlace);
		Renderer->DrawRectangle
		(
			intrect
			(
				TextBlock->GetLetterPosition(Area.FromPlace),
				int2(FirstWidth, TextBlock->GetLineHeight())
			),
			SelectionColor
		);

		uint LastWidth = TextBlock->GetLineFormerWidth(Area.ToPlace);
		Renderer->DrawRectangle
		(
			intrect
			(
				TextBlock->GetLetterPosition(SRTextPlace(Area.ToPlace.Line, 0)),
				int2(LastWidth, TextBlock->GetLineHeight())
			),
			SelectionColor
		);

		for (int i = Area.FromPlace.Line + 1; i < Area.ToPlace.Line; i++)
		{
			Renderer->DrawRectangle
			(
				intrect
				(
					TextBlock->GetLetterPosition(SRTextPlace(i, 0)),
					int2(TextBlock->GetLineWidth(i), TextBlock->GetLineHeight())
				),
				SelectionColor
			);
		}
	}
}

void FUITextBox::OnRendering()
{
	FUIImage::OnRendering();

	if (IsTextSelected)
	{
		auto Area = SRTextPlaceArea(CursorSelectFrom, Cursor);
		RenderTextArea(Renderer, TextBlock, Area, float4(0.1, 0.3, 1, 0.5));
	}
	
	if (IsCursorVisible)
	{
		Renderer->DrawRectangle
		(
			intrect
			(
				TextBlock->GetLetterPosition(Cursor),
				int2(3, TextBlock->GetLineHeight())
			),
			float4(1)
		);
	}
	IsCursorVisible = !IsCursorVisible;
	RegisterTimer(GetCaretBlinkTime());	
}

void FUITextBox::OnKeyDown(wchar_t Key)
{
	switch (Key)
	{
	case VK_UP:
		if (_IsMultiLine)
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
		if (_IsMultiLine)
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
		if (_IsMultiLine)
		{		
			if (IsTextSelected)DeleteSelectedArea();
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
				if (_IsMultiLine && Cursor.Line > 0)
				{
					Cursor = TextBlock->LinkPreviusLine(Cursor.Line);
				}
			}
			else Cursor = TextBlock->DeleteLetter(Cursor);			
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
	if (Char == L'\b' || Char == L'\r' || Char == L'\n')return;
	if (IsTextSelected)DeleteSelectedArea();
	Cursor = TextBlock->InsertLetter(Cursor, Char);
	UpdateAndCursorFlash();
}

void FUITextBox::OnMouseDown(EMouseButton Button, int2 Position)
{
	IsMousePressed = true;
	CursorSelectFrom = Cursor = TextBlock->GetTextPlace(Position);
	UpdateAndCursorFlash();
}

void FUITextBox::OnMouseUp(EMouseButton Button, int2 Position)
{
	IsMousePressed = false;
}

void FUITextBox::OnMouseMove(int2 Position)
{
	ChangeCursor(ECursorType::IBeam);
	if (IsMousePressed)
	{
		Cursor = TextBlock->GetTextPlace(Position);
		IsTextSelected = CursorSelectFrom.Line != Cursor.Line || CursorSelectFrom.Letter != Cursor.Letter;
		UpdateAndCursorFlash();
	}
	RegisterTrackMouseLeave();
}

void FUITextBox::OnMouseLeave(int2 Position)
{
	IsTextSelected = false;
	IsMousePressed = false;
	UpdateAndCursorFlash();
}

void FUITextBox::OnTimer()
{
	Update();
}

void FUITextBox::UpdateAndCursorFlash()
{	
	IsCursorVisible = true;	
	SetAllItemCount(TextBlock->GetLineCount());
	Update();
	if(_OnTextChanged) _OnTextChanged();
}

void FUITextBox::DeleteSelectedArea()
{
	IsTextSelected = false;
	auto Area = SRTextPlaceArea(CursorSelectFrom, Cursor);
	Cursor = TextBlock->DeleteLettersBetween(Area);
}

void FUITextBox::SetLineOffset(uint StartLineIndex)
{
	TextBlock->SetLineOffset(StartLineIndex);
}

FUITextEditor::FUITextEditor()
{
	TextBox = new FUITextBox();	
	UIBase
	.ScrolledControl(TextBox)
	.ScrollableControl(TextBox);
}