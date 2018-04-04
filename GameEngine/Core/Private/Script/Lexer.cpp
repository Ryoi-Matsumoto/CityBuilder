#include "..\..\Public\Script\Lexer.h"
#include <ccType>
#include <locale>

void SLexer::Analyze(const char* Source)
{
	SToken Stock;
	bool SureString = false;
	bool StringSymbol = false;
	bool charSymbol = false;

	auto TokenUp = [&](SToken x)
	{
		if (StringSymbol && Stock.Annotator == EAnnotator::String)
			Errors.push_back(SError(Stock.Location, "終わりの\"がありません。"));
		else if (charSymbol && Stock.Annotator == EAnnotator::Char)
			Errors.push_back(SError(Stock.Location, "終わりの'がありません。"));
		else Tokens.push_back(Stock);
	};

	int Cursor = 0, Line = 1, Index = 0;
	int Lenght = strlen(Source);
	for (; Cursor < Lenght; Cursor++, Index++)
	{
		char Current = Source[Cursor];
		if (Current == '\n')
		{
			Line++;
			Index = 0;
		}

		// コメント
		bool hitcomment = false;/*
		for(auto Comment : Comments)
		{
			bool ok = true;
			for (int i = 0, j = Cursor; i < strlen(Comment.Open); i++, j++)
			{
				if (Source[j] != Comment.Open[i])
				{
					ok = false;
					break;
				}
			}
			if (ok)
			{
				hitcomment = true;
				Cursor = (int)strstr(Source + Cursor, Comment.End);
				if (Cursor < 0)
				{
					if (Stock.Annotator != EAnnotator::Empty) TokenUp(Stock);
					return;
				}
				break;
			}
		}*/
		if (!hitcomment)
		{
			// "\"
			if (Current == '\\' && !SureString && Stock.Annotator != EAnnotator::Empty && (Stock.Annotator == EAnnotator::Char || Stock.Annotator == EAnnotator::String))
			{
				SureString = true;
			}
			// 文字列の"
			else if (Current == StringEnclosed && !SureString)// 前に\がないとき
			{
				StringSymbol = !StringSymbol;
				if (Stock.Annotator != EAnnotator::Empty)
				{
					TokenUp(Stock);
					if (Stock.Annotator == EAnnotator::String) Stock.Annotator = EAnnotator::Empty;
					else Stock = SToken(EAnnotator::String, SLocation(Line, Index, Cursor));
				}
				else
				{
					Stock = SToken(EAnnotator::String, SLocation(Line, Index, Cursor));
				}
			}
			// 文字の'
			else if (Current == CharEnclosed && !SureString)// 前に\がないとき
			{
				charSymbol = !charSymbol;
				if (Stock.Annotator != EAnnotator::Empty)
				{
					TokenUp(Stock);
					if (Stock.Annotator == EAnnotator::Char) Stock.Annotator = EAnnotator::Empty;
					else Stock = SToken(EAnnotator::Char, SLocation(Line, Index, Cursor));
				}
				else
				{
					Stock = SToken(EAnnotator::Char, SLocation(Line, Index, Cursor));
				}
			}
			// "と"の間
			else if (Stock.Annotator == EAnnotator::String)
			{
				SureString = false;
				Stock.Add(Current);
			}
			// 'と'の間
			else if (Stock.Annotator == EAnnotator::Char)
			{
				SureString = false;
				Stock.Add(Current);
			}
			// アルファベット
			else if (std::isalpha(Current))
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
			// 数字
			else if (std::isdigit(Current))
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
			// ドット
			else if (Current == '.')
			{
				if (Stock.Annotator != EAnnotator::Empty)
				{
					// 小数点としてのドット
					if (Stock.Annotator == EAnnotator::Integer)
					{
						Stock.Annotator = EAnnotator::Decimal;
						Stock.Add(Current);
					}
					// 記号としてのドット
					else
					{
						TokenUp(Stock);
						Tokens.push_back(SToken(Current, EAnnotator::Symbol, SLocation(Line, Index, Cursor)));
						Stock.Annotator = EAnnotator::Empty;
					}
				}
				else Tokens.push_back(SToken(Current, EAnnotator::Symbol, SLocation(Line, Index, Cursor)));
			}
			// 空白,改行,タブ
			else if (strchr(SpaceChars, Current) != nullptr)
			{
				if (Stock.Annotator != EAnnotator::Empty)
				{
					TokenUp(Stock);
					Stock.Annotator = EAnnotator::Empty;
				}
			}
			// 記号
			else if (strchr(Symbols, Current) != nullptr)
			{
				if (Stock.Annotator != EAnnotator::Empty)
				{
					TokenUp(Stock);
					Stock.Annotator = EAnnotator::Empty;
				}
				Tokens.push_back(SToken(Current, EAnnotator::Symbol, SLocation(Line, Index, Cursor)));
			}
			// 演算子
			else if (strchr(Operetors, Current) != nullptr)
			{
				if (Stock.Annotator != EAnnotator::Empty)
				{
					if (Stock.Annotator == EAnnotator::Operetor)
					{
						Stock.Add(Current);
					}
					else
					{
						TokenUp(Stock);
						Stock = SToken(Current, EAnnotator::Operetor, SLocation(Line, Index, Cursor));
					}
				}
				else Stock = SToken(Current, EAnnotator::Operetor, SLocation(Line, Index, Cursor));
			}
			// エラー
			else
			{
				if (Stock.Annotator != EAnnotator::Empty)
				{
					if (Stock.Annotator == EAnnotator::Error)
					{
						Stock.Add(Current);
					}
					else
					{
						TokenUp(Stock);
						Stock = SToken(Current, EAnnotator::Error, SLocation(Line, Index, Cursor));
					}
				}
				else
				{
					if (Stock.Annotator != EAnnotator::Empty) Errors.push_back(SError(Stock.Location, "無効なトークンです。"));
					Stock = SToken(Current, EAnnotator::Error, SLocation(Line, Index, Cursor));
				}
			}
		}
	}
	if (Stock.Annotator != EAnnotator::Empty) TokenUp(Stock);
	Tokens.push_back(SToken("", EAnnotator::End, SLocation(Line, Index, Cursor)));

	for (int i = 0; i < Tokens.size(); i++)
	{
		Tokens[i].String[Tokens[i].Location.Length] = '\0';
	}
}