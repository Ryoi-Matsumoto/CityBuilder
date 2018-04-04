#pragma once

struct SLocation
{
	SLocation()
	{
	}

	SLocation(int Line, int IndexOfLine, int Index, int Lenght = 0)
		: Line(Line), IndexOfLine(IndexOfLine), Index(Index), Length(Length)
	{
	}

	int Line;
	int Index;
	int IndexOfLine;
	int Length;
	
	static SLocation Between(SLocation a, SLocation b)
	{
		return SLocation(a.Line, a.IndexOfLine, a.Index, b.Index - a.Index + b.Length);
	}
};

struct SError
{
	SError(SLocation InLocation, string msg)
		:Location(InLocation), Messeage(msg)
	{
	}

	string Messeage;
	SLocation Location;
};

enum class EAnnotator
{
	Empty, Identifier, Integer, Symbol, Decimal, String, Char, Operetor, Error, End, Boolean, Any
};

struct SToken
{
	SToken()
		: Annotator(EAnnotator::Empty)
	{
	}

	SToken(char* InString, EAnnotator InAnnotator, SLocation InLocation)
	{
		strcpy(String, InString);
		Annotator = InAnnotator;
		Location = InLocation;
		Location.Length = strlen(InString);
	}

	SToken(char c, EAnnotator InAnnotator, SLocation InLocation)
	{
		String[0] = c;
		String[1] = '\0';
		Annotator = InAnnotator;
		Location = InLocation;
		Location.Length = 1;
	}

	SToken(EAnnotator InAnnotator, SLocation InLocation)
	{
		String[0] = '\0';
		Annotator = InAnnotator;
		Location = InLocation;
		Location.Length = 0;
	}

	SToken(string InString)
	{
		strcpy(String, &InString[0]);
		Location.Length = InString.size();
	}

	void Add(char Char)
	{
		String[Location.Length] = Char;
		Location.Length++;
	}

	char String[32];
	EAnnotator Annotator;
	SLocation Location;
};

struct SComment
{
	SComment(char* InOpen, char* InEnd)
	{
		strcpy(Open, InOpen);
		strcpy(End, InEnd);
	}

	char Open[4];
	char End[4];
};

struct SLexer
{
	char Symbols[32];
	char Operetors[32];
	char SpaceChars[32];
	char StringEnclosed;
	char CharEnclosed;
	vector<SComment> Comments;
	
	static SLexer Default()
	{
		SLexer Result;
		strcpy(Result.Symbols, "{}()[];:,,");
		strcpy(Result.Operetors, "+-*\<>=%");
		strcpy(Result.SpaceChars, " \r\t\v\n\f");
		Result.StringEnclosed = '"';
		Result.CharEnclosed = '\'';
		Result.Comments = vector<SComment>{ SComment("//", "\n"), SComment("/*", "*/") };
		return Result;
	}

	vector<SToken> Tokens;
	vector<SError> Errors;

	void Analyze(const char* Source);
};