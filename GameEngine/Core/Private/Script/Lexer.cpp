#include "..\..\Public\Script\Lexer.h"
#include <ccType>
#include <locale>

void SLexer::Analyze(const char* Source)
{
	SToken Stock;
	bool IsSureString = false;
	bool IsStringSymbol = false;
	bool IsCharSymbol = false;

	auto TokenUp = [&](SToken x)
	{
		switch (Stock.Annotator)
		{
		case EAnnotator::String:
			if (IsStringSymbol)
			{
				Stock.Annotator = EAnnotator::Error;
				Errors.push_back(SError(Stock.Location, "�I����\"������܂���B"));
			}
			break;
		case EAnnotator::Char:
			if (IsCharSymbol)
			{
				Stock.Annotator = EAnnotator::Error;
				Errors.push_back(SError(Stock.Location, "�I����'������܂���B"));
			}
			break;
		case EAnnotator::Integer:
		case EAnnotator::Decimal:
			/*
			if (Stock.Location.Length > 9)
			{
				Stock.Annotator = EAnnotator::Error;
				Errors.push_back(SError(Stock.Location, "���l���������܂��B"));
			}
			*/
			break;
		}
		Tokens.push_back(Stock);
	};

	const uint Lenght = strlen(Source);
	uint Cursor = 0, Line = 1, Index = 0;

	auto CheckComment = [&](SComment Comment)
	{
		if (Source[Cursor] == Comment1.Open[0] && (Comment1.Open[1] == '\0' || (Cursor + 1 < Lenght && Source[Cursor + 1] == Comment1.Open[1])))
		{
			if (Stock.Annotator != EAnnotator::Empty)
				TokenUp(Stock);
			Cursor = (int)strstr(Source + Cursor, Comment1.End);
			if (Cursor < 0)
			{
				Errors.push_back(SError(Stock.Location, "�t�@�C���̖����ŃR�����g�������Ă��܂���B"));
				Cursor = Lenght;
			}
			return true;
		}

		return false;
	};

	for (; Cursor < Lenght; Cursor++, Index++)
	{
		char Current = Source[Cursor];

		// ���s
		if (Current == '\n')
		{
			Line++;
			Index = 0;
		}
		// �R�����g
		else if (CheckComment(Comment1));
		else if (CheckComment(Comment2));
		// "\"
		else if (Current == '\\' && !IsSureString && Stock.Annotator != EAnnotator::Empty && (Stock.Annotator == EAnnotator::Char || Stock.Annotator == EAnnotator::String))
		{
			IsSureString = true;
		}
		// �������"
		else if (Current == StringEnclosed && !IsSureString)// �O��\���Ȃ��Ƃ�
		{
			IsStringSymbol = true;
			switch (Stock.Annotator)
			{
			case EAnnotator::Empty:
				Stock = SToken(EAnnotator::String, SLocation(Line, Index, Cursor));
				break;
			case EAnnotator::String:
				IsStringSymbol = false;
				TokenUp(Stock);
				Stock.Annotator = EAnnotator::Empty;
				break;
			default:
				TokenUp(Stock);
				Stock = SToken(EAnnotator::String, SLocation(Line, Index, Cursor));
			}
		}
		// ������'
		else if (Current == CharEnclosed && !IsSureString)// �O��\���Ȃ��Ƃ�
		{
			IsCharSymbol = true;
			switch (Stock.Annotator)
			{
			case EAnnotator::Empty:
				Stock = SToken(EAnnotator::Char, SLocation(Line, Index, Cursor));
				break;
			case EAnnotator::Char:
				IsStringSymbol = false;
				TokenUp(Stock);
				Stock.Annotator = EAnnotator::Empty;
				break;
			default:
				TokenUp(Stock);
				Stock = SToken(EAnnotator::Char, SLocation(Line, Index, Cursor));
			}
		}
		// "��"�̊�
		else if (Stock.Annotator == EAnnotator::String)
		{
			IsSureString = false;
			Stock.Add(Current);
		}
		// '��'�̊�
		else if (Stock.Annotator == EAnnotator::Char)
		{
			IsSureString = false;
			Stock.Add(Current);
		}
		// �A���t�@�x�b�g
		else if (isalpha(Current) || Current == '_')
		{
			switch (Stock.Annotator)
			{
			case EAnnotator::Empty:
				Stock = SToken(Current, EAnnotator::Identifier, SLocation(Line, Index, Cursor));
				break;
			case EAnnotator::Identifier:
				Stock.Add(Current); break;
				break;
			default:
				TokenUp(Stock);
				Stock = SToken(Current, EAnnotator::Identifier, SLocation(Line, Index, Cursor));
			}
		}
		// ����
		else if (isdigit(Current))
		{
			switch (Stock.Annotator)
			{
			case EAnnotator::Empty:
				Stock = SToken(Current, EAnnotator::Integer, SLocation(Line, Index, Cursor));
				break;
			case EAnnotator::Identifier:
			case EAnnotator::Integer:
			case EAnnotator::Decimal:
				Stock.Add(Current); break;
				break;
			default:
				TokenUp(Stock);
				Stock = SToken(Current, EAnnotator::Integer, SLocation(Line, Index, Cursor));
			}
		}
		// �h�b�g
		else if (Current == '.')
		{
			switch (Stock.Annotator)
			{
			case EAnnotator::Empty:
				Tokens.push_back(SToken(Current, EAnnotator::Symbol, SLocation(Line, Index, Cursor)));
				break;
			case EAnnotator::Integer:
				Stock.Annotator = EAnnotator::Decimal;
				Stock.Add(Current);
				break;
			default:
				TokenUp(Stock);
				Tokens.push_back(SToken(Current, EAnnotator::Symbol, SLocation(Line, Index, Cursor)));
				Stock.Annotator = EAnnotator::Empty;
			}
		}
		// ��,���s,�^�u
		else if (strchr(SpaceChars, Current) != nullptr)
		{
			switch (Stock.Annotator)
			{
			case EAnnotator::Empty:
				break;
			default:
				TokenUp(Stock);
				Stock.Annotator = EAnnotator::Empty;
			}
		}
		// �L��
		else if (strchr(Symbols, Current) != nullptr)
		{
			switch (Stock.Annotator)
			{
			case EAnnotator::Empty:
				break;
			default:
				TokenUp(Stock);
				Stock.Annotator = EAnnotator::Empty;
			}
			Tokens.push_back(SToken(Current, EAnnotator::Symbol, SLocation(Line, Index, Cursor)));
		}
		// ���Z�q
		else if (strchr(Operetors, Current) != nullptr)
		{
			switch (Stock.Annotator)
			{
			case EAnnotator::Empty:
				Stock = SToken(Current, EAnnotator::Operetor, SLocation(Line, Index, Cursor));
				break;
			/*
			case EAnnotator::Operetor:
				Stock.Add(Current);
				break;*/
			default:
				TokenUp(Stock);
				Stock = SToken(Current, EAnnotator::Operetor, SLocation(Line, Index, Cursor));
			}
		}
		// �G���[
		else
		{
			switch (Stock.Annotator)
			{
			case EAnnotator::Empty:
				Stock = SToken(Current, EAnnotator::Error, SLocation(Line, Index, Cursor));
				break;
			case EAnnotator::Error:
				Stock.Add(Current);
				break;
			default:
				TokenUp(Stock);
				Stock = SToken(Current, EAnnotator::Error, SLocation(Line, Index, Cursor));
				Errors.push_back(SError(Stock.Location, "�����ȃg�[�N���ł��B"));
			}
		}

	}

	if (Stock.Annotator != EAnnotator::Empty) 
		TokenUp(Stock);

	for (uint i = 0; i < Tokens.size(); i++)
		Tokens[i].String[Tokens[i].Location.Length] = '\0';
}