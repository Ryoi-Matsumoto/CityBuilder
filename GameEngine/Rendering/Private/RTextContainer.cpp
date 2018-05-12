#include "..\Public\RTextContainer.h"

uint FRTextContainer::GetLineBetweenWidth(SRTextPlace Start, uint Offset) const
{
	uint Width = 0;
	for (uint i = Start.Letter; i < Start.Letter + Offset; i++)
	{
		Width += Lines[Start.Line][i]->GetWidth();
	}
	return Width;
}

uint FRTextContainer::GetLineWidth(uint Line) const
{
	return GetLineBetweenWidth(SRTextPlace(Line, 0), Lines[Line].size());
}

uint FRTextContainer::GetLineFormerWidth(SRTextPlace Place) const 
{ 
	return GetLineBetweenWidth(SRTextPlace(Place.Line, 0), Place.Letter);
}

uint FRTextContainer::GetLineLatterWidth(SRTextPlace Place) const 
{ 
	return GetLineBetweenWidth(Place, Lines[Place.Line].size() - Place.Letter); 	 
}

uint FRTextContainer::GetLetterNumber(uint Line, int PositionX) const
{
	uint LetterNumber = 0;
	uint LetterCount = Lines[Line].size();

	if (LetterCount > 0)
	{
		LetterNumber = LetterCount;
		int LetterPositionX = 0, PrevLetterPositionX = 0;
		for (int i = 0; i < LetterCount; i++)
		{
			LetterPositionX += Lines[Line][i]->GetWidth();
			if (LetterPositionX > PositionX)
			{
				if ((LetterPositionX + PrevLetterPositionX) / 2.0 > PositionX)
				{
					LetterNumber = i;
				}
				else LetterNumber = i + 1;
				break;
			}
			PrevLetterPositionX = LetterPositionX;
		}
	}
	return LetterNumber;
}

FRTextContainer::FRTextContainer(FRFontSet* FontSet)
	: FontSet(FontSet)
	, LineHeight(FontSet->GetFontType().Size)
	, StartLine(0)
{
}

uint FRTextContainer::GetLineNumber(int PositionY) const
{
	uint Line = StartLine + PositionY / LineHeight;
	Line = FMath::Min(Line, Lines.size() - 1);
	return Line;
}

int2 FRTextContainer::GetLetterPosition(SRTextPlace Place) const
{
	int2 Position(0, LineHeight * 0.1f + LineHeight * (Place.Line - StartLine));

	auto& CurrentLine = Lines[Place.Line];
	for (uint i = 0; i < Place.Letter; i++)
		Position.X += CurrentLine[i]->GetWidth();
	return Position;
}

SRTextPlace FRTextContainer::InsertLetter(SRTextPlace Place, wchar_t Letter)
{
	auto& CurrentLine = Lines[Place.Line];
	auto NewFont = FontSet->GetFontLetter(Letter);
	CurrentLine.insert(CurrentLine.begin() + Place.Letter, NewFont);
	Place.Letter++;
	return Place;
}

SRTextPlace FRTextContainer::InsertReturn(SRTextPlace Place)
{
	auto& CurrentLine = Lines[Place.Line];
	vector<FRFontLetter*> FormerLine, LatterLine;

	for (uint i = 0; i < Place.Letter; i++)
		FormerLine.push_back(CurrentLine[i]);
	for (uint i = Place.Letter; i < CurrentLine.size(); i++)
		LatterLine.push_back(CurrentLine[i]);

	Lines.erase(Lines.begin() + Place.Line);
	Lines.insert(Lines.begin() + Place.Line, FormerLine);
	Lines.insert(Lines.begin() + Place.Line + 1, LatterLine);

	Place.Line++;
	Place.Letter = 0;
	return Place;
}

SRTextPlace FRTextContainer::DeleteLetter(SRTextPlace Place)
{
	auto& CurrentLine = Lines[Place.Line];
	CurrentLine.erase(CurrentLine.begin() + Place.Letter - 1);
	Place.Letter--;
	return Place;
}

SRTextPlace FRTextContainer::DeleteLettersBetween(SRTextPlaceArea Area)
{
	auto& FirstLine = Lines[Area.FromPlace.Line];
	if (Area.FromPlace.Line == Area.ToPlace.Line)
	{
		auto From = FirstLine.begin() + Area.FromPlace.Letter;
		auto To = FirstLine.begin() + Area.ToPlace.Letter;
		FirstLine.erase(From, To);
	}
	else
	{	
		FirstLine.erase(FirstLine.begin() + Area.FromPlace.Letter, FirstLine.end());

		auto& LastLine = Lines[Area.ToPlace.Line];	
		if (LastLine.size() == Area.ToPlace.Letter)
		{
			Area.ToPlace.Line++;
			Area.ToPlace.Letter = 0;
		}
		else
		{
			LastLine.erase(LastLine.begin(), LastLine.begin() + Area.ToPlace.Letter);
		}

		for (uint i, FirstLine = i = Area.FromPlace.Line + 1; i < Area.ToPlace.Line; i++)
			Lines.erase(Lines.begin() + FirstLine);

		if (Area.FromPlace.Line < Lines.size() - 1)
			LinkPreviusLine(Area.FromPlace.Line + 1);
	}
	return Area.FromPlace;
}

SRTextPlace FRTextContainer::LinkPreviusLine(uint Line)
{
	auto& CurrentLine = Lines[Line];
	auto& PrevLine = Lines[Line - 1];
	auto PrevLineSize = PrevLine.size();
	for (uint i = 0; i < CurrentLine.size(); i++)
	{
		PrevLine.push_back(CurrentLine[i]);
	}
	Lines.erase(Lines.begin() + Line);

	SRTextPlace Place;
	Place.Letter = PrevLineSize;
	Place.Line = Line - 1;
	return Place;
}

SRTextPlace FRTextContainer::DragAndDropText(SRTextPlaceArea Area, SRTextPlace DragTo)
{
	vector<FRFontLetter*> DraggedLetters;
	for (uint Line = Area.FromPlace.Line; Line < Area.ToPlace.Line; Line++)
	{
		for (uint Letter = 0; Letter < Lines[Line].size(); Letter++)
		{
			DraggedLetters.push_back(Lines[Line][Letter]);
		}
	}

	return DragTo;
}

SRTextPlace FRTextContainer::GetTextPlace(int2 Position) const
{
	SRTextPlace Place;
	Place.Line = GetLineNumber(Position.Y);
	Place.Letter = GetLetterNumber(Place.Line, Position.X);
	return Place;
}

void FRTextContainer::SetText(wstring Text)
{
	Lines.clear();

	if (Text.size() == 0)
	{
		vector<FRFontLetter*> EmptyLine;
		Lines.push_back(EmptyLine);
	}
	else
	{
		vector<FRFontLetter*> FontLetter;
		for (auto Char : Text)
		{
			if (Char == L'\n')
			{
				Lines.push_back(FontLetter);
				FontLetter.clear();
			}
			else if (Char != L'\r')
			{
				FontLetter.push_back(FontSet->GetFontLetter(Char));
			}
		}
		Lines.push_back(FontLetter);
	}
}

wstring FRTextContainer::GetText() const
{
	uint TextCount = Lines.size();
	for (auto& Line : Lines)
		TextCount += Line.size();

	wstring Text;
	Text.reserve(TextCount);

	for (auto& Line : Lines)
	{
		for (auto Letter : Line)
			Text += Letter->GetLetter();
		Text += L'\n';
	}

	return Text;
}