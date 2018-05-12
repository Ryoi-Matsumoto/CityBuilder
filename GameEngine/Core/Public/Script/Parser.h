#pragma once

#define _ParserOperator \
template<typename TAdd> \
constexpr auto operator + (TAdd Right) const \
{ \
	return SParserAdd<remove_pointer_t<decltype(this)>, TAdd, SPair<TResult, TAdd::TResult>, 2>(); \
} \
template<typename TAdd> \
constexpr auto operator | (TAdd Right) const \
{ \
	return SParserOr<remove_pointer_t<decltype(this)>, TAdd, TResult>(); \
}

struct ParsedData
{
	vector<SToken> Tokens;
	void* Marks[32];
	void* Mark;
	int Index;
	ParsedData(vector<SToken>&& Tokens) : Tokens(move(Tokens)), Index(0) {}
};

template<typename TLambda, typename TResult, typename TArgument>
inline TResult GetLambdaFunction(TArgument Argument)
{
	TResult(TLambda::*Function)(TArgument) const = &TLambda::operator();
	char Memory[sizeof(TLambda) / sizeof(char)];
	TLambda* LambdaPointer = (TLambda*)&Memory;
	return (LambdaPointer->*Function)(Argument);
}

template<typename T, typename _TResult, typename TLambda>
struct SParserAction
{
	_ParserOperator;
	using TResult = _TResult;
	static bool Parse(ParsedData& Data, TResult* Out)
	{
		T::TResult Result;

		if (!T::Parse(Data, &Result))
			return false;

		*Out = GetLambdaFunction<TLambda, TResult, T::TResult>(Result);
		return true;
	}
};

template<uint64 String>
struct SParserString
{
	_ParserOperator;
	using TResult = SToken;	
	static bool Parse(ParsedData& Data, SToken* Out)
	{
		if (Data.Tokens.size() <= Data.Index)
			return false;

		uint64 StringCopy = String;

		if(!strcmp(Data.Tokens[Data.Index].String, (const char*)&StringCopy))
		{
			*Out = Data.Tokens[Data.Index];
			Data.Index += 1;
			return true;
		}

		return false;
	}
};

template<EAnnotator Annotator>
struct SParserAnnotator
{
	_ParserOperator;
	using TResult = SToken;
	static bool Parse(ParsedData& Data, SToken* Out)
	{
		if (Data.Tokens.size() <= Data.Index)
			return false;

		if (Annotator == EAnnotator::Any || Data.Tokens[Data.Index].Annotator == Annotator)
		{
			*Out = Data.Tokens[Data.Index];
			Data.Index += 1;
			return true;
		}

		return false;
	}
};

template<typename TLeft, typename TRight>
struct SPair
{	
	TLeft Left;
	TRight Right;

	constexpr SPair() {}
	constexpr SPair(TLeft Left, TRight Right) : Left(Left), Right(Right) {}
};

template<typename TLeft, typename TRight, typename _TResult>
struct SParserOr : public SPair<TLeft, TRight>
{
	_ParserOperator;
	using TResult = _TResult;
	static bool Parse(ParsedData& Data, TResult* Out)
	{
		if (TLeft::Parse(Data, Out))
			return true;

		if (TRight::Parse(Data, Out))
			return true;

		return false;
	}
};

template<typename T, typename TSplitter, typename _TResult>
struct SParserSplit
{
	_ParserOperator;
	using TResult = _TResult;
	static bool Parse(ParsedData& Data, TResult* Out)
	{
		TResult Results;
		TResult::value_type Result;
		TSplitter::TResult Dammy;

		if (T::Parse(Data, &Result))
			Out->push_back(Result);
		else
			return true;

		while (Data.Index < Data.Tokens.size())
		{
			auto Index = Data.Index;
			if (!TSplitter::Parse(Data, &Dammy))
				break;
			if (!T::Parse(Data, &Result))
			{
				Data.Index = Index;
				break;
			}
			Out->push_back(Result);
		}

		return true;
	}
};

template<typename T, typename TSplitter, typename _TResult>
struct SParserSplitCatenation
{
	_ParserOperator;
	using TResult = _TResult;
	static bool Parse(ParsedData& Data, TResult* Out)
	{
		int Backup = Data.Index;
		TResult Results;
		TResult::value_type Result;
		TSplitter::TResult Dammy;

		if (T::Parse(Data, &Result))
			Out->push_back(Result);
		else
			return false;

		while (Data.Index < Data.Tokens.size())
		{
			auto Index = Data.Index;
			if (!TSplitter::Parse(Data, &Dammy))
				break;
			if (!T::Parse(Data, &Result))
			{
				Data.Index = Index;
				break;
			}
			Out->push_back(Result);
		}
		
		if (Out->size() == 1)
		{
			Data.Index = Backup;
			return false;
		}

		return true;
	}
};

template<typename T, typename _TResult>
struct SParserAtLeastOnce
{
	_ParserOperator;
	using TResult = _TResult;
	static bool Parse(ParsedData& Data, TResult* Out)
	{
		auto BackupIndex = Data.Index;

		while (Data.Index < Data.Tokens.size())
		{
			TResult::value_type Result;

			if (!T::Parse(Data, &Result))
				break;

			Out->push_back(Result);
		}

		if (Results.size() == 0)
		{
			Data.Index = BackupIndex;
			return false;
		}

		return true;
	}
};

template<typename T, typename _TResult>
struct SParserMany
{
	_ParserOperator;
	using TResult = _TResult;

	static bool Parse(ParsedData& Data, TResult* Out)
	{
		TResult Results;

		while (Data.Index < Data.Tokens.size())
		{
			TResult::value_type Result;

			if (!T::Parse(Data, &Result))
				break;

			Out->push_back(Result);
		}

		return true;
	}
};

template<typename T, typename _TResult>
struct SParserNoneOrOnce
{
	_ParserOperator;
	using TResult = _TResult;

	static bool Parse(ParsedData& Data, TResult* Out)
	{
		TResult Result;

		if (T::Parse(Data, &Result))
			*Out = Result;
		else
			memset(Out, 0, sizeof(TResult));

		return true;
	}
};

template<typename T, typename _TResult>
struct SParserNoneOrOncePair
{
	_ParserOperator;
	using TResult = _TResult;

	static bool Parse(ParsedData& Data, TResult* Out)
	{
		T::TResult Result;

		if (T::Parse(Data, &Result))
			*Out = TResult(Result, true);
		else
			*Out = TResult(TResult::first_type(), false);

		return true;
	}
};

template<typename T, typename _TResult>
struct SParserNoneOrOnceVector
{
	_ParserOperator;
	using TResult = _TResult;
	static bool Parse(ParsedData& Data, vector<TResult> Out)
	{
		vector<TResult> Result;

		if (T::Parse(Data, &Result))
			*Out = Result;
		else
			*Out = vector<TResult>();

		return true;
	}
};

template<typename T>
struct SParserIsThere
{
	_ParserOperator;
	using TResult = bool;
	static bool Parse(ParsedData& Data, bool* Out)
	{
		T::TResult Result;

		if (T::Parse(Data, &Result))
			*Out = true;
		else
			*Out = false;

		return true;
	}
};

template<typename T, typename _TResult, uint Mark>
struct SMarker
{
	_ParserOperator;
	using TResult = _TResult;
	static bool Parse(ParsedData& Data, TResult* Out)
	{
		Data.Marks[Mark] = (void*)&T::Parse;
		return T::Parse(Data, Out);
	}
};

template<typename _TResult, uint Mark>
struct SParserReference
{
	_ParserOperator;
	using TResult = _TResult;
	static bool Parse(ParsedData& Data, TResult* Out)
	{
		return ((bool(*)(ParsedData& Data, TResult* Out))Data.Marks[Mark])(Data, Out);
	}
};

template<typename _TResult>
struct SParserRecursion
{
	_ParserOperator;
	using TResult = _TResult;
	static bool Parse(ParsedData& Data, TResult* Out)
	{
		return ((bool(*)(ParsedData& Data, TResult* Out))Data.Mark)(Data, Out);
	}
};

template<typename T, typename _TResult>
struct SParserSettingMarker
{
	_ParserOperator;
	using TResult = _TResult;
	static bool Parse(ParsedData& Data, TResult* Out)
	{
		Data.Mark = (void*)&T::Parse;
		return T::Parse(Data, Out);
	}
};

template<typename TLeft, typename TRight, typename _TResult, uint NumAdd>
struct SParserAddBase
{
	static const uint NumAdd = NumAdd;
	using TResult = _TResult;
	static bool Parse(ParsedData& Data, TResult* Out)
	{
		int Index = Data.Index;

		if (!TLeft::Parse(Data, &Out->Left))
			return false;

		if (!TRight::Parse(Data, &Out->Right))
		{
			Data.Index = Index;
			return false;
		}

		return true;
	}
};

template<typename TLeft, typename TRight, typename TResult, uint NumAdd>
struct SParserAdd : public SParserAddBase<TLeft, TRight, TResult, NumAdd> {};

#define _DefParserAdd(N) \
template<typename TLeft, typename TRight, typename TResult> \
struct SParserAdd<TLeft, TRight, TResult, N> : public SParserAddBase<TLeft, TRight, TResult, N>

#define _DefParserAddOperator \
template<typename TAdd> \
constexpr auto operator + (TAdd Right) const \
{ \
	using TNewResult = SPair<TResult, TAdd::TResult>; \
	return SParserAdd<remove_pointer_t<decltype(this)>, TAdd, TNewResult, NumAdd + 1>(); \
}

#define _DefResult(N, E) \
static auto GetResult##N(TResult Result) { return Result.E; }

#define _x0 Right
#define _x1 Left.Right
#define _x2 Left.Left.Right
#define _x3 Left.Left.Left.Right
#define _x4 Left.Left.Left.Left.Right
#define _x5 Left.Left.Left.Left.Left.Right
#define _x6 Left.Left.Left.Left.Left.Left.Right
#define _x7 Left.Left.Left.Left.Left.Left.Left.Right

#define _x1e Left
#define _x2e Left.Left
#define _x3e Left.Left.Left
#define _x4e Left.Left.Left.Left
#define _x5e Left.Left.Left.Left.Left
#define _x6e Left.Left.Left.Left.Left.Left

_DefParserAdd(2)
{
	_DefParserAddOperator;
	_DefResult(0, _x1e);
	_DefResult(1, _x0);
};

_DefParserAdd(3)
{
	_DefParserAddOperator;
	_DefResult(0, _x2e);
	_DefResult(1, _x1);
	_DefResult(2, _x0);
};

_DefParserAdd(4)
{
	_DefParserAddOperator;
	_DefResult(0, _x3e);
	_DefResult(1, _x2);
	_DefResult(2, _x1);
	_DefResult(3, _x0);
};

_DefParserAdd(5)
{
	_DefParserAddOperator;
	_DefResult(0, _x4e);
	_DefResult(1, _x3);
	_DefResult(2, _x2);
	_DefResult(3, _x1);
	_DefResult(4, _x0);
};

_DefParserAdd(6)
{
	_DefParserAddOperator;
	_DefResult(0, _x5e);
	_DefResult(1, _x4);
	_DefResult(2, _x3);
	_DefResult(3, _x2);
	_DefResult(4, _x1);
	_DefResult(5, _x0);
};

_DefParserAdd(7)
{
	_DefParserAddOperator;
	_DefResult(0, _x6e);
	_DefResult(1, _x5);
	_DefResult(2, _x4);
	_DefResult(3, _x3);
	_DefResult(4, _x2);
	_DefResult(5, _x1);
	_DefResult(6, _x0);
};

template<typename T, typename TConnecter>
struct SParserCatenationResult
{
	using TResult = SPair<SPair<T, TConnecter>, T>;
	_DefResult(0, _x2e);
	_DefResult(1, _x1);
	_DefResult(2, _x0);
};

template<typename T, typename TConnecter, typename _TResult, typename TLambda>
struct SParserCatenation
{
	_ParserOperator;
	using TResult = _TResult;
	static bool Parse(ParsedData& Data, TResult* Out)
	{
		TResult Result;

		if (!T::Parse(Data, &Result))
			return false;

		while (Data.Index < Data.Tokens.size())
		{
			TConnecter::TResult Connecter;
			TResult Body;
			auto BackupIndex = Data.Index;

			if (!TConnecter::Parse(Data, &Connecter))
				break;

			if (!T::Parse(Data, &Body))
			{
				Data.Index = BackupIndex;
				break;
			}

			using TPrimaryResult = SPair<SPair<TResult, TConnecter::TResult>, TResult>;
			auto Pair = TPrimaryResult(SPair<TResult, TConnecter::TResult>(Result, Connecter), Body);
			Result = GetLambdaFunction<TLambda, TResult, TPrimaryResult>(Pair);
		}

		*Out = Result;
		return true;
	}
};

namespace Single
{
	const SParserAnnotator<EAnnotator::Any> Any;
	const SParserAnnotator<EAnnotator::Identifier> Ident;
	const SParserAnnotator<EAnnotator::Integer> Integer;
	const SParserAnnotator<EAnnotator::Symbol> Symbol;
	const SParserAnnotator<EAnnotator::Decimal> Decimal;
	const SParserAnnotator<EAnnotator::String> String;
	const SParserAnnotator<EAnnotator::Char> Char;
	const SParserAnnotator<EAnnotator::Operetor> Operetor;
	const SParserAnnotator<EAnnotator::Error> Error;
}

template<typename T>
inline auto IsThere(T Target)
{
	return SParserIsThere<T>(Target);
}

template<typename T>
inline auto Many(T Target)
{
	return SParserMany<T, vector<T::TResult>>();
}

template<typename T, typename TSplitter>
inline auto Split(T Target, TSplitter Splitter)
{
	return SParserSplit<T, TSplitter, vector<T::TResult>>();
}

template<typename T, typename TSplitter>
inline auto SplitCatenation(T Target, TSplitter Splitter)
{
	return SParserSplitCatenation<T, TSplitter, vector<T::TResult>>();
}

template<typename T>
inline auto AtLeastOnce(T Target)
{
	return SParserAtLeastOnce<T, vector<T::TResult>>();
}

template<typename T>
inline auto NoneOrOnce(T Target)
{
	return SParserNoneOrOnce<T, T::TResult>();
}

template<typename T>
inline auto NoneOrOncePair(T Target)
{
	return SParserNoneOrOncePair<T, pair<T::TResult, bool>>();
}

template<typename T>
inline auto RootParse(T Parser, vector<SToken>&& Tokens)
{
	ParsedData Data(move(Tokens));
	T::TResult Result;
	T::Parse(Data, &Result);
	return Result;
}

#define Act(P, R) \
([=]() { \
	auto Parser = P; \
	auto Lambda = [](decltype(Parser)::TResult $){return R; }; \
	decltype(Parser)::TResult Test; \
	return SParserAction<decltype(Parser), decltype(Lambda(Test)), decltype(Lambda)>(); \
})()

#define Catenation(B, C, R) \
([=]() { \
	auto Body = B; \
	auto Connecter = C; \
	auto Parser = SParserCatenationResult<decltype(Body)::TResult, decltype(Connecter)::TResult>(); \
	auto Lambda = [](decltype(Parser)::TResult $){return R; }; \
	return SParserCatenation<decltype(Body), decltype(Connecter), decltype(Body)::TResult, decltype(Lambda)>(); \
})()

#define PLit(S) SParserString<ToUint64(S)>()
#define PSet(P) SParserSettingMarker<decltype(P), decltype(P)::TResult>()
#define PMark(P, M) SMarker<decltype(P), decltype(P)::TResult, (uint)M>()
#define PRef(T, M) SParserReference<T, (uint)M>()
#define PRec(T) SParserRecursion<T>()
#define PCombi(A, B) Act(A + B, SToken(strcat($0.String, $1.String), $0.Annotator, SLocation::Between($0.Location, $1.Location)))

#define $0 decltype(Parser)::GetResult0($)
#define $1 decltype(Parser)::GetResult1($)
#define $2 decltype(Parser)::GetResult2($)
#define $3 decltype(Parser)::GetResult3($)
#define $4 decltype(Parser)::GetResult4($)
#define $5 decltype(Parser)::GetResult5($)
#define $6 decltype(Parser)::GetResult6($)