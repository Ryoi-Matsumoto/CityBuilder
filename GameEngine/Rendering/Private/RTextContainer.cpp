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

uint FRTextContainer::GetLetterNumber(uint Line, uint PositionX) const
{
	uint LetterNumber;
	uint LetterCount = Lines[Line].size();

	if (LetterCount == 0)
	{
		LetterNumber = 0;
	}
	else
	{
		LetterNumber = LetterCount;
		int LetterPositionX = 0, PrevLetterPositionX = 0;
		for (int i = 0; i < LetterCount - 1; i++)
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

uint FRTextContainer::GetLineNumber(uint PositionY) const
{
	uint Line = StartLine + PositionY / LineHeight;
	Line = FMath::Min(Line, Lines.size() - 1);
	return Line;
}

int2 FRTextContainer::GetLetterPosition(SRTextPlace Place) const
{
	int2 Position(0, LineHeight * (Place.Line - StartLine));
	auto& CurrentLine = Lines[Place.Line];
	for (int i = 0; i < Place.Letter; i++)
	{
		Position.X += CurrentLine[i]->GetWidth();
	}
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
	for (int i = 0; i < Place.Letter; i++)
	{
		FormerLine.push_back(CurrentLine[i]);
	}
	for (int i = Place.Letter; i < CurrentLine.size(); i++)
	{
		LatterLine.push_back(CurrentLine[i]);
	}
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

		for (int i, FirstLine = i = Area.FromPlace.Line + 1; i < Area.ToPlace.Line; i++)
		{
			Lines.erase(Lines.begin() + FirstLine);
		}
		if (Area.FromPlace.Line < Lines.size() - 1)
		{
			LinkPreviusLine(Area.FromPlace.Line + 1);
		}
	}
	return Area.FromPlace;
}

SRTextPlace FRTextContainer::LinkPreviusLine(uint Line)
{
	auto& CurrentLine = Lines[Line];
	auto& PrevLine = Lines[Line - 1];
	auto PrevLineSize = PrevLine.size();
	for (int i = 0; i < CurrentLine.size(); i++)
	{
		PrevLine.push_back(CurrentLine[i]);
	}
	Lines.erase(Lines.begin() + Line);

	SRTextPlace Place;
	Place.Letter = PrevLineSize;
	Place.Line = Line - 1;
	return Place;
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
		vector<FRFontLetter*> Fontwchar_ts;
		for (auto Char : Text)
		{
			if (Char == L'\n')
			{
				Lines.push_back(Fontwchar_ts);
				Fontwchar_ts.clear();
			}
			else if (Char != L'\r')
			{
				Fontwchar_ts.push_back(FontSet->GetFontLetter(Char));
			}
		}
		Lines.push_back(Fontwchar_ts);
	}
}