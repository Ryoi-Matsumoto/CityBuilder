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
	auto Rule = Act(Expr + PLit("=") + Expr + PLit(";"), FRule($0.Normalize(), $2));
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

void Math(string Expr, string Rule)
{
	auto Lexer = SLexer::Default();
	Lexer.Analyze(Expr.data());
	auto Result = RootParse(GetExprParser(), move(Lexer.Tokens));
	
	//auto RuleText = FFileManager::ReadTextFile("C:\\Users\\RYOUI\\Desktop\\CityBuilder\\GameEngine\\Math\\Private\\Rule.txt");
	//Lexer.Analyze(&RuleText.data());
	Lexer.Analyze(Rule.data());
	auto RuleTable = RootParse(GetRulesParser(), move(Lexer.Tokens));

	auto Manipulation = Result.Calculate(RuleTable);
	cout << Result.ToString();
	for (auto& Process : Manipulation.GetProcesses())
	{
		cout << "\n= " + Process.Expression.ToString();
	}
	cout << "\n\n";
}

auto MathCalculate()
{
	auto Text = "log(2,6*8)+log(2,48)";
	auto Rule = FFileManager::ReadTextFile("C:\\Users\\RYOUI\\Desktop\\CityBuilder\\GameEngine\\Math\\Private\\Rule.txt");

	Math("a-(-b-c)", Rule);
	for (;;);
	Math(Text, Rule);
	Text = "log(2,432+43-324/432*492/(948-42)-321)";
	Math(Text, Rule);

	Math("x^2+6*x+8", "x^2+(2+n)*x+2*n=(x+2)*(x+n);");
	
	Text = "x^2+6*x+8";
	Math(Text, Rule);

	Text = "integral(4*x^2+2*x+56,x)";
	Math(Text, Rule);

	Text = "integral(log(2,x)^n,x)";
	Math(Text, Rule);

	Text = "integral(log(2,x+3)^n,x+3)";
	Math(Text, Rule);
	Math("factorial(10)", "factorial(1)=1;factorial(n)=n*factorial(n-1);");

	for (;;);
}
/*

0‚Í”‚ð•\‹L‚·‚éÛ‚É•SŒÜ\ŽO‚âˆê–œŒÜ\ŽO‚ð‹æ•Ê‚·‚éÛ‚É‚Í10053‚È‚Ç‚ÆA‚»‚ÌŒ…‚É”‚ª‚È‚¢‚±‚Æ‚ðŽ¦‚·•K—v‚ª‚ ‚éB

2-5‚Æ‚¢‚Á‚½ˆø‚«ŽZ‚ÌŒ‹‰Ê-3‚È‚Ç‚Æ‚¢‚¤•‰‚Ì”‚ÌŠT”O‚ª‚Å‚«‚½

3*3*3‚Æ‚¢‚Á‚½Š|‚¯ŽZ‚ÌŒJ‚è•Ô‚µ‚ðŠÈ—ª‚µ‚Ä•\‹L‚·‚é‚½‚ß‚É2^3‚Æ‚¢‚Á‚½‚×‚«æ‚ÌŠT”O‚ª‚Å‚«‚½B
‚»‚ê‚ÉŽŸ‚¢‚ÅAx^3=8‚Æ‚¢‚Á‚½x=sqrt(3,8)‚ð
2^x=8 x=log(2,8)


a*x^2+b*x+c=
x^2+(n*m)*x+(n+m)=(x+n)*(x+m);

x2+6x+8

b=n+m c=n*m





6 8
3 6

(n+1) n
x^2+n*x+n=x*(x
x^2+(2+n)*x+2*n=(x+2)*(x+n);


*/
