#include "..\..\Public\Script\Json.h"
#include "..\..\Public\Script\Lexer.h"
#include "..\..\Public\Script\Parser.h"

auto GetParser()
{
	enum class Marker
	{
		Block, Value, End
	};		

	auto Array = Act(Literal("[") + Split(Ref(SDynamic, Marker::Value), Literal(",")) + Literal("]"), SDynamic(move($1)));
	auto Value2 = Act(Single::Integer, SDynamic((SDynamic::TInteger)stoi($.String)))
		| Act(Single::Decimal, SDynamic((SDynamic::TDecimal)stod($.String)))
		| Act(Single::String, SDynamic(string($.String)))
		| Act(Literal("true"), SDynamic(true))
		| Act(Literal("false"), SDynamic(false))
		| Act(Literal("null"), SDynamic())
	    | Act(Ref(SDynamic, Marker::Block), $)
		| Act(Array, SDynamic($));
	auto Value = Mark(Value2, (uint)Marker::Value);

	auto Pair = Act(Single::String + Literal(":") + Value, SDynamic::TPair($0.String, $2));

	auto BlockImp = Act(Literal("{") + Split(Pair, Literal(",")) + Literal("}"), SDynamic(move($1)));
	auto Block = Mark(BlockImp, (uint)Marker::Block);
	
	return Block;
}

auto Parser = GetParser();

SJson::SJson(const char* Text)
{
	auto Lexer = SLexer::Default();
	Lexer.Analyze(Text);
	IsError = !Parser.Parse(ParsedData(move(Lexer.Tokens)), this);
}