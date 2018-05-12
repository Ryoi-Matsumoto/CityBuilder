#include "MIntermediate.h"
#include "MExpression.h"
#include "MTerm.h"
#include <iostream>

SRule ToRule(SIntermediate* Left, SIntermediate* Right)
{
	SRule Rule;
	Rule.Left = Left->Calculate();
	Rule.Right = Right->Calculate();
	return Rule;
}

auto GetExprParser()
{
	auto RefExpr = PRef(SIntermediate*, 0);
	auto Value = Act(Single::Integer, new SIntermediate((int)stoi($.String)));
	auto Variable = Act(Single::Ident, new SIntermediate($.String));
	auto Arguments = Act(PLit("(") + Split(RefExpr, PLit(",")) + PLit(")"), $1);
	auto Function = Act(Single::Ident + Arguments, new SIntermediate($0.String, $1));
	auto Bracket = Act(PLit("(") + RefExpr + PLit(")"), $1) | Value | Function | Variable;
	auto Pow = Catenation(Bracket, PLit("^"), new SIntermediate(EIntermediateType::Pow, $0, $2));
	auto Unary = Act(PLit("-") + Pow, new SIntermediate(EIntermediateType::Minus, $1)) | Pow;
	auto MulOpe = Act(PLit("*"), EIntermediateType::Mul) | Act(PLit("/"), EIntermediateType::Div);
	auto MulExpr = Catenation(Unary, MulOpe, new SIntermediate($1, $0, $2));
	auto AddOpe = Act(PLit("+"), EIntermediateType::Add) | Act(PLit("-"), EIntermediateType::Sub);
	auto AddExpr = Catenation(MulExpr, AddOpe, new SIntermediate($1, $0, $2));
	return PMark(AddExpr, 0);
}

auto GetRulesParser()
{
	auto Expr = GetExprParser();
	auto Rule = Act(Expr + PLit("=") + Expr + PLit(";"), ToRule($0, $2));
	return Many(Rule);
}

void Math(string Expr)
{
	auto Lexer = SLexer::Default();
	Lexer.Analyze(&Expr[0]);
	auto Result = RootParse(GetExprParser(), move(Lexer.Tokens));
	/*
	auto RuleText = FFileManager::ReadTextFile("C:\\Users\\RYOUI\\Desktop\\CityBuilder\\GameEngine\\Math\\Private\\Rule.txt");
	Lexer.Analyze(&RuleText[0]);
	auto Rules = RootParse(GetRulesParser(), move(Lexer.Tokens));
	*/
	auto Terms = FExpression(Result->Calculate());

	cout << Terms.ToString();
}

auto MathCalculate()
{
	//auto Log = "log(a,a)=1";
	auto Text = "sin((6+3)*pi)+2*x+4*sin(3^2*pi)+x";

	Math(Text);

	for (;;);
}