#pragma once
#include "..\Public\RFontLetter.h"

struct SRTextPlace
{
	uint Line = 0;
	uint Letter = 0;

	SRTextPlace()
	{
	}

	SRTextPlace(uint Line, uint Letter)
		: Line(Line)
		, Letter(Letter)
	{
	}

	bool operator <(SRTextPlace Right)
	{
		if (Line == Right.Line)return Letter < Right.Letter;
		return Line < Right.Line;
	}

	bool operator >(SRTextPlace Right)
	{
		if (Line == Right.Line)return Letter > Right.Letter;
		return Line > Right.Line;
	}

	bool operator ==(SRTextPlace Right)
	{
		return Line == Right.Line && Letter == Right.Letter;
	}

	bool operator !=(SRTextPlace Right)
	{
		return Line != Right.Line || Letter != Right.Letter;
	}

	static SRTextPlace Near(SRTextPlace Left, SRTextPlace Right)
	{
		if (Left < Right)return Left;
		return Right;
	}

	static SRTextPlace Far(SRTextPlace Left, SRTextPlace Right)
	{
		if (Left > Right)return Left;
		return Right;
	}
};

struct SRTextPlaceArea
{	
	SRTextPlace FromPlace;
	SRTextPlace ToPlace;

	SRTextPlaceArea(SRTextPlace Place0, SRTextPlace Place1)
	{
		if (Place0 < Place1)
		{
			FromPlace = Place0;
			ToPlace = Place1;
		}
		else
		{
			FromPlace = Place1;
			ToPlace = Place0;
		}
	}
};

class FRTextContainer
{
private:
	vector<vector<FRFontLetter*>> Lines;
	uint LineHeight;
	FRFontSet* FontSet;
	uint StartLine;

public:
	FRTextContainer(FRFontSet* FontSet);
	void SetText(wstring Text);	
	uint GetLineNumber(uint PositionY) const;
	uint GetLineCount() const { return Lines.size(); }
	uint GetLineHeight() const { return LineHeight; }
	uint GetLineWidth(uint Line) const;
	uint GetLineBetweenWidth(SRTextPlace Start, uint Offset) const;
	uint GetLineFormerWidth(SRTextPlace Place) const;
	uint GetLineLatterWidth(SRTextPlace Place) const;	
	uint GetLetterNumber(uint Line, uint PositionX) const;
	uint GetLetterCount(uint Line) const { return Lines[Line].size(); }	
	int2 GetLetterPosition(SRTextPlace Place) const;	
	SRTextPlace GetTextPlace(int2 Position) const;	
	SRTextPlace InsertLetter(SRTextPlace Place, wchar_t Letter);
	SRTextPlace InsertReturn(SRTextPlace Place);
	SRTextPlace DeleteLetter(SRTextPlace Place);
	SRTextPlace DeleteLettersBetween(SRTextPlaceArea Area);
	SRTextPlace LinkPreviusLine(uint Line);
	vector<vector<FRFontLetter*>> const& GetLines() const { return Lines; }
	void SetLineOffset(uint _StartLine) { _StartLine = StartLine; }
	uint GetLineOffset() const { return StartLine; }
};