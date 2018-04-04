#include "..\Public\FrontEnd.h"
#include "Statement.h"
#include "Expression.h"

enum class Marker
{
	Expression, Statement, StatementBlock, StatementFunction
};

auto GetTypeParser()
{
	return Act(Literal("int"), EValueType::Int)
		| Act(Literal("int2"), EValueType::Int2)
		| Act(Literal("int3"), EValueType::Int3)
		| Act(Literal("int3x4"), EValueType::Int3x4)
		| Act(Literal("int3x8"), EValueType::Int3x8)
		| Act(Literal("real"), EValueType::Real)
		| Act(Literal("real2"), EValueType::Real2)
		| Act(Literal("real3"), EValueType::Real3)
		| Act(Literal("real3x4"), EValueType::Real3x4)
		| Act(Literal("real3x8"), EValueType::Real3x8)
		| Act(Literal("bool"), EValueType::Boolean);
}

auto GetValueParser()
{
	auto Int = Act(Single::Integer, (FExpression*)new FExpressionLiteral($, EValueType::Int));
	auto Real = Act(Single::Decimal, (FExpression*)new FExpressionLiteral($, EValueType::Real));
	auto Bool = Act(Literal("true") | Literal("false"), (FExpression*)new FExpressionLiteral($, EValueType::Boolean));
	auto Variable = Act(Single::Ident, (FExpression*)new FExpressionVariable($));
	return Int | Real | Bool | Variable;
}

auto GetExpressionParser()
{
	auto Value = GetValueParser();

	auto Expression = Rec(FExpression*);
	auto Arguments = Act(Literal("(") + Split(Expression, Literal(",")) + Literal(")"), $1);
	auto Function = Act(Single::Ident + Arguments, (FExpression*)new FExpressionFunctionCall($0, $1));
	auto Bracket = Function | Act(Literal("(") + Expression + Literal(")"), $1) | Value;

	auto Unary = Act((Literal("-") | Literal("!")) + Bracket, (FExpression*)new FExpressionUnaryOperation($0, $1)) | Bracket;
	auto MulExpr = Catenation(Unary, Literal("*") | Literal("/"), (FExpression*)new FExpressionOperation($0, $1, $2));
	auto AddExpr = Catenation(MulExpr, Literal("+") | Literal("-"), (FExpression*)new FExpressionOperation($0, $1, $2));
	auto CmpExpr = Catenation(AddExpr, Literal("<") | Literal("<=") | Literal(">") | Literal(">="), (FExpression*)new FExpressionOperation($0, $1, $2));
	auto LgcExpr = Catenation(CmpExpr, Literal("&&") | Literal("||"), (FExpression*)new FExpressionOperation($0, $1, $2));

	return Marker(LgcExpr);
}

auto GetStatementBlockParser()
{
	auto Type = GetTypeParser();
	auto Expression = GetExpressionParser();

	auto RefStatement = Ref(FStatement*, Marker::Statement);
	auto BracketBlockImp = Literal("{") + Many(RefStatement) + Literal("}");
	auto BracketBlock = Act(BracketBlockImp, new FStatementBlock($1));
	auto SimpleBlockImp = Literal("=>") + RefStatement;
	auto SimpleBlock = Act(SimpleBlockImp, new FStatementBlock(vector<FStatement*>({ $1 })));
	auto Block = SimpleBlock | BracketBlock;

	auto Assign = Act(Single::Ident + Single::Operetor + Expression, (FStatement*)new FStatementAssignation($0, $1, $2));
	auto Declar = Act(Type + Single::Ident + NoneOrOnce(Act(Literal("=") + Expression, $1)), (FStatement*)new FStatementDeclaration($0, $1, $2));
	auto Return = Act(Literal("return") + Expression, (FStatement*)new FStatementReturn($1));

	auto IfImp = Act(Literal("if") + Literal("(") + Expression + Literal(")") + Block, new SStatementIfBlock($0.Location, $2, $4));
	auto ElifImp = Act(Literal("elif") + Literal("(") + Expression + Literal(")") + Block, new SStatementIfBlock($0.Location, $2, $4));
	auto ElseImp = Act(Literal("else") + Block, $1);
	auto If = Act(IfImp + Many(ElifImp) + NoneOrOnce(ElseImp), (FStatement*)new FStatementIf($0, $1, $2));

	auto WhileImp = Literal("while") + Literal("(") + Expression + Literal(")") + Block;
	auto While = Act(WhileImp, (FStatement*)new FStatementWhile($0.Location, $2, $4));

	auto ArgumentDeclar = Act(Type + Single::Ident, new FStatementDeclaration($0, $1, nullptr));
	auto Arguments = Act(Literal("(") + Split(ArgumentDeclar, Literal(",")) + Literal(")"), $1);

	auto ProcedureImp = Literal("proc") + Single::Ident + Literal(":") + Single::Ident + Arguments + Block;
	auto Procedure = Act(ProcedureImp, (FStatement*)new FStatementDefineProcedure($1, $3, $4, $5));

	auto Function = Act(Type + Single::Ident + Arguments + Block, (FStatement*)new FStatementDefineFunction($0, $1, $2, $3));

	auto CallArguments = Act(Literal("(") + Split(Expression, Literal(",")) + Literal("("), $1);
	auto CallForkBlock = Act(Literal(":") + Expression + Block, SForkBlock($1, $2));
	auto CallProcedure = Act(Single::Ident + CallArguments + Many(CallForkBlock), (FStatement*)new FStatementProcedure($0, $1, $2));

	auto Statement = Return | Declar | Assign | If | While | CallProcedure | Procedure | Function;

	return Many(Mark(Statement, Marker::Statement));
}

vector<SInstruction> GenerateIntermediate(string Source)
{
	auto Lexer = SLexer::Default();
	Lexer.Analyze(&Source[0]);
	auto Statements = RootParse(GetStatementBlockParser(), move(Lexer.Tokens));

	SCompilingData Data;
	FIntermediate Intermediate;
	FRoutine Routine(nullptr);
	FScope Scope(nullptr);
	Data.Intermediate = &Intermediate;
	Data.Routine = &Routine;
	Data.Scope = &Scope;

	for (auto Statement : Statements)Statement->GenerateCode(Data, nullptr);
	return Routine.GetInstructions();
}