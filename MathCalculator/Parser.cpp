#include "..\GameEngine\Core\Public\System\Template.h"
#include "..\GameEngine\Core\Public\System\Constexpr.h"
#include "..\GameEngine\Core\Public\Script\Lexer.h"
#include "..\GameEngine\Core\Public\Script\Parser.h"

void aa()
{
	auto Value = Act(Single::Integer | Single::Decimal, stod($.String));
	auto Bracket = Act(PLit("(") + PRef(double, 0) + PLit(")"), $1) | Value;
	auto Unary = Act(PLit("-") + Bracket, -$1) | Bracket;
	auto MulExpr = Catenation(Unary, Act(PLit("*"), true) | Act(PLit("/"), false), $1 ? $0 * $2 : $0 / $2);
	auto AddExpr = Catenation(MulExpr, Act(PLit("+"), true) | Act(PLit("-"), false), $1 ? $0 + $2 : $0 - $2);
	double ExprResult = DefaultParse(PMark(AddExpr, 0), "2*-2");
}