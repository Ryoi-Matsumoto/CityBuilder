#include "..\Public\MExpression.h"
#include <iostream>

auto GetExprParser()
{
	auto RefExpr = PRef(FExpression, 0);
	auto Value = Act(Single::Integer, FExpression(mint($.String)));
	auto Variable = Act(Single::Ident, FExpression(EExpressionType::Variable, $.String));
	auto Arguments = Act(PLit("(") + Split(RefExpr, PLit(",")) + PLit(")"), $1);
	auto Function = Act(Single::Ident + Arguments, FExpression(EExpressionType::Function, $0.String, $1));
	auto Bracket = Act(PLit("(") + RefExpr + PLit(")"), $1) | Value | Function | Variable;
	auto Pow = Act(SplitCatenation(Bracket, PLit("^")), FExpression(EExpressionType::Pow, $)) | Bracket;
	auto Mns = Act(PLit("-") + Pow, FExpression(EExpressionType::Mul, vector<FExpression>{FExpression(-1), $1})) | Pow;
	auto Div = Catenation(Mns, PLit("/"), FExpression(EExpressionType::Div, { $0, $2 }));
	auto Mul = Catenation(Div, PLit("*"), FExpression(EExpressionType::Mul, { $0, $2 }));
	auto Sub = Catenation(Mul, PLit("-"), FExpression(EExpressionType::Sub, { $0, $2 }));
	auto Add = Catenation(Sub, PLit("+"), FExpression(EExpressionType::Add, { $0, $2 }));
	return PMark(Add, 0);
}

auto GetRulesParser()
{
	auto Expr = GetExprParser();
	auto Inequal = Act(PLit("<"), EInequalityType::Less) | Act(PLit(">"), EInequalityType::Greater)
		| Act(PCombi(PLit("<"), PLit("=")), EInequalityType::LessEqual) 
		| Act(PCombi(PLit(">"), PLit("=")), EInequalityType::GreaterEqual)
		| Act(PCombi(PLit("!"), PLit("=")), EInequalityType::NotEqual);
	auto Range = Act(Single::Ident + Inequal + Expr, SVariableInequality($0.String, SInequality($1, $2)));
	auto Condition = Act(NoneOrOnce(PLit(":") + Split(Range, PLit(","))), $.Right);
	auto Rule = Act(Expr + PLit("=") + Expr + Condition + PLit(";"), FRule($0.Normalize(), $2, $3));
	return Act(Many(Rule), FRuleTable($));
}

FExpression::FExpression(string Text)
{
	auto Lexer = SLexer::Default();
	Lexer.Analyze(Text.data());
	auto TokenCount = Lexer.Tokens.size();
	auto Data = ParsedData(move(Lexer.Tokens));
	GetExprParser().Parse(Data, this);	
	if (Data.Index != TokenCount)
		Type = EExpressionType::Empty;
}

FRuleTable::FRuleTable(string Text)
{
	auto Lexer = SLexer::Default();
	Lexer.Analyze(Text.data());
	auto Data = ParsedData(move(Lexer.Tokens));
	GetRulesParser().Parse(Data, this);
}