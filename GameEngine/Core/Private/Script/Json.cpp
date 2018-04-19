#include "..\..\Public\Script\Json.h"
#include "..\..\Public\Script\Lexer.h"
#include "..\..\Public\Script\Parser.h"

auto GetParser()
{
	enum class Marker
	{
		Block, Value, End
	};		

	auto Array = Act(PLit("[") + Split(PRef(SDynamic, Marker::Value), PLit(",")) + PLit("]"), SDynamic(move($1)));
	auto Value2 = Act(Single::Integer, SDynamic((SDynamic::TInteger)stoi($.String)))
		| Act(Single::Decimal, SDynamic((SDynamic::TDecimal)stod($.String)))
		| Act(Single::String, SDynamic(string($.String)))
		| Act(PLit("true"), SDynamic(true))
		| Act(PLit("false"), SDynamic(false))
		| Act(PLit("null"), SDynamic())
		| Act(PRef(SDynamic, Marker::Block), $)
		| Act(Array, SDynamic($));
	auto Value = PMark(Value2, Marker::Value);

	auto Pair = Act(Single::String + PLit(":") + Value, SDynamic::TPair($0.String, $2));

	auto BlockImp = Act(PLit("{") + Split(Pair, PLit(",")) + PLit("}"), SDynamic(move($1)));
	auto Block = PMark(BlockImp, Marker::Block);
	
	return Block;
}

auto Parser = GetParser();

SJson::SJson(const char* Text)
{
	auto Lexer = SLexer::Default();
	Lexer.Analyze(Text);
	IsError = !Parser.Parse(ParsedData(move(Lexer.Tokens)), this);
}