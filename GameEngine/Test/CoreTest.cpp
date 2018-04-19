#include "Test.h"
#include "Core\Public\Script\Lexer.h"
#include "Core\Public\Script\Parser.h"
#include "Core\Public\Script\Json.h"

template<typename T>
auto DefaultParse(T Parser, const char* Text)
{
	auto Lexer = SLexer::Default();
	Lexer.Analyze(Text);
	return RootParse(Parser, move(Lexer.Tokens));
}

void ParserTest()
{
	// 2項Addテスト
	auto Parser = Single::Operetor + Single::String;
	auto Result = DefaultParse(Parser, "+\"apple\"");
	assert(!strcmp(Result.Left.String, "+") && !strcmp(Result.Right.String, "apple"));

	// 3項Addテスト
	auto Parser2 = Single::String + PLit(":") + Single::Integer;
	auto Result2 = DefaultParse(Parser2, "\"lenght\":12");
	assert(!strcmp(Result2.Right.String, "12"));

	// Orテスト
	auto Parser22 = Single::String | Single::Symbol | Single::Integer;
	auto Result22 = DefaultParse(Parser22, "12");
	assert(!strcmp(Result22.String, "12"));

	// Actテスト
	auto Parser3 = Act(Parser2, stoi($2.String));
	auto Result3 = DefaultParse(Parser3, "\"lenght\":12");
	assert(Result3 == 12);
	
	// 再帰テスト
	auto ParserInt = Act(Single::Integer, stoi($.String));
	auto Parser4 = Act(Split(ParserInt | PRef(int, 0), PLit(",")), accumulate($.begin(), $.end(), 0));
	auto Parser5 = Act(PLit("[") + Parser4 + PLit("]"), $1);
	auto Parser6 = PMark(Parser5, 0);
	auto Result4 = DefaultParse(Parser6, "[[1,2,3,[4,5,6],7,8,9],[1,2,3,[4,[5,[6,7,[8],9]]]]]");
	assert(Result4 == 90);

	// Jsonテスト
	SJson Json("{\"windows\":7,\"pi\":3.14,\"fruit\":\"apple\",\"is_json\":true}");
	assert(!Json.IsError);
	assert(Json["windows"].GetInteger() == 7);
	assert(Json["pi"].GetDecimal() == 3.14f);
	assert(Json["fruit"].GetString() == "apple");
	assert(Json["is_json"].GetBoolean());

	SJson Json2("{\"array\":[0,1,2],\"object\":{\"iphone\":10}}");
	assert(!Json2.IsError);
	assert(Json2["array"][2].GetInteger() == 2);
	assert(Json2["object"]["iphone"].GetInteger() == 10);

	// 四則計算
	auto Value   = Act(Single::Integer | Single::Decimal, stod($.String));
	auto Bracket = Act(PLit("(") + PRef(double, 0) + PLit(")"), $1) | Value;
	auto Unary   = Act(PLit("-") + Bracket, -$1) | Bracket;
	auto MulExpr = Catenation(Unary,   Act(PLit("*"), true) | Act(PLit("/"), false), $1 ? $0 * $2 : $0 / $2);
	auto AddExpr = Catenation(MulExpr, Act(PLit("+"), true) | Act(PLit("-"), false), $1 ? $0 + $2 : $0 - $2);
	double ExprResult = DefaultParse(PMark(AddExpr, 0), "2*-2");
	//assert(abs(ExprResult - 42) < 0.1);
}