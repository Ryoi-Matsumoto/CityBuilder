#include "..\Public\FrontEnd.h"
#include "Statement.h"
#include "Expression.h"

enum class Marker
{
	Expression, Statement
};

auto GetTypeParser()
{
	return Act(PLit("int"), SType::Int)
		| Act(PLit("int2"), SType::Int2)
		| Act(PLit("int3"), SType::Int3)
		| Act(PLit("int4"), SType::Int4)
		| Act(PLit("int4x2"), SType::Int4x2)
		| Act(PLit("real"), SType::Real)
		| Act(PLit("real2"), SType::Real2)
		| Act(PLit("real3"), SType::Real3)
		| Act(PLit("real3x4"), SType::Real3x4)
		| Act(PLit("real3x4x2"), SType::Real3x4x2)
		| Act(PLit("bool"), SType::Bool);
}

auto GetValueParser()
{
	auto Int = Act(Single::Integer, (FExpression*)new FExpressionLiteral($, EValueType::Int));
	auto Real = Act(Single::Decimal, (FExpression*)new FExpressionLiteral($, EValueType::Real));
	auto Bool = Act(PLit("true") | PLit("false"), (FExpression*)new FExpressionLiteral($, EValueType::Bool));
	auto Variable = Act(Single::Ident, (FExpression*)new FExpressionVariable($));
	return Int | Real | Bool | Variable;
}

auto GetExpressionParser()
{
	auto Value = GetValueParser();

	auto Expression = PRec(FExpression*);
	auto Arguments = Act(PLit("(") + Split(Expression, PLit(",")) + PLit(")"), $1);
	auto Function = Act(Single::Ident + Arguments, (FExpression*)new FExpressionFunctionCall($0, $1));
	auto Bracket = Function | Act(PLit("(") + Expression + PLit(")"), $1) | Value;

	auto Unary = Act((PLit("-") | PLit("!")) + Bracket, (FExpression*)new FExpressionUnaryOperation($0, $1)) | Bracket;
	auto MulExpr = Catenation(Unary, PLit("*") | PLit("/"), (FExpression*)new FExpressionOperation($0, $1, $2));
	auto AddExpr = Catenation(MulExpr, PLit("+") | PLit("-"), (FExpression*)new FExpressionOperation($0, $1, $2));
	auto CmpExprImp = PCombi(PLit("<"), PLit("=")) | PLit("<") | PCombi(PLit(">"), PLit("=")) | PLit(">");
	auto CmpExpr = Catenation(AddExpr, CmpExprImp, (FExpression*)new FExpressionOperation($0, $1, $2));
	auto LgcExpr = Catenation(CmpExpr, PLit("&&") | PLit("||"), (FExpression*)new FExpressionOperation($0, $1, $2));

	return PSet(LgcExpr);
}

auto GetStatementBlockParser()
{
	auto Type = GetTypeParser();
	auto Expression = GetExpressionParser();

	auto RefStatement = PRef(FStatement*, Marker::Statement);
	auto BracketBlockImp = PLit("{") + Many(RefStatement) + PLit("}");
	auto BracketBlock = Act(BracketBlockImp, new FStatementBlock($1));
	auto SimpleBlockImp = PLit("=") + PLit(">") + RefStatement;
	auto SimpleBlock = Act(SimpleBlockImp, new FStatementBlock(vector<FStatement*>({ $2 })));
	auto Block = SimpleBlock | BracketBlock;

	auto Assign = Act(Single::Ident + Single::Operetor + Expression, (FStatement*)new FStatementAssignation($0, $1, $2));
	auto Declar = Act(Type + Single::Ident + NoneOrOnce(Act(PLit("=") + Expression, $1)), (FStatement*)new FStatementDeclaration($0, $1, $2));
	auto Return = Act(PLit("return") + Expression, (FStatement*)new FStatementReturn($1));

	auto IfImp = Act(PLit("if") + PLit("(") + Expression + PLit(")") + Block, new SStatementIfBlock($0.Location, $2, $4));
	auto ElifImp = Act(PLit("elif") + PLit("(") + Expression + PLit(")") + Block, new SStatementIfBlock($0.Location, $2, $4));
	auto ElseImp = Act(PLit("else") + Block, $1);
	auto If = Act(IfImp + Many(ElifImp) + NoneOrOnce(ElseImp), (FStatement*)new FStatementIf($0, $1, $2));

	auto WhileImp = PLit("while") + PLit("(") + Expression + PLit(")") + Block;
	auto While = Act(WhileImp, (FStatement*)new FStatementWhile($0.Location, $2, $4));

	auto ArgumentDeclar = Act(Type + Single::Ident, new FStatementDeclaration($0, $1, nullptr));
	auto Arguments = Act(PLit("(") + Split(ArgumentDeclar, PLit(",")) + PLit(")"), $1);

	auto ForkYieldType = NoneOrOncePair(Act(PLit("[") + Type + PLit("]"), $1));
	auto ProcedureImp = PLit("def") + Single::Ident + PLit(":") + Single::Ident + Arguments + ForkYieldType + Block;
	auto Procedure = Act(ProcedureImp, (FStatement*)new FStatementDefineProcedure($1, $3, $4, $5.first, !$5.second, $6));

	auto Function = Act(Type + Single::Ident + Arguments + Block, (FStatement*)new FStatementDefineFunction($0, $1, $2, $3));

	using TPair = pair<SType, SToken>;
	auto Member = Act(Type + Single::Ident, TPair($0, $1));
	auto ShapeImp = PLit("shape") + Single::Ident + Act(PLit("{") + Many(Member) + PLit("}"), $1);
	auto Shape = Act(ShapeImp, (FStatement*)new FStatementDefineShape($1, $2));

	auto CallArguments = Act(PLit("(") + Split(Expression, PLit(",")) + PLit(")"), $1);
	auto CallForkBlock = Act(PLit(":") + Expression + Block, SForkBlock($1, $2));
	auto CallProcedure = Act(Single::Ident + CallArguments + Many(CallForkBlock), (FStatement*)new FStatementProcedure($0, $1, $2));

	auto InvaildToken = Act(Single::Any, (FStatement*)new FStatementInvaildToken($));

	auto Statement = Procedure | Function | Shape | Return | If | While | Declar | Assign | CallProcedure | InvaildToken;

	return Many(PMark(Statement, Marker::Statement));
}

void InitializeBuiltIn(FScope* Scope)
{
	Scope->AddFunction
	(
		"push_vertex",
		SType::Int3,
		vector<SType>({ SType::Real3x3 }),
		[](const SCompilingData& Data, const vector<regptr>& Arguments)
		{
			auto StackPointer = Data.Scope->GetStackPointer();
			Data.Routine->AddInstruction(EInstruction::PUSHI, SType::Real3x3, StackPointer, Arguments[0], 0);
			return StackPointer;
		}
	);

	Scope->AddProcedure
	(
		"push_index", 
		vector<SType>({ SType::Int3 }),
		[](const SCompilingData& Data, const vector<regptr>& Arguments)
		{
			Data.Routine->AddInstruction(EInstruction::PUSHV, SType::Int3, Arguments[0], 0, 0);
		}
	);
}

SIntermediate GenerateIntermediate(string Source)
{
	SIntermediate Result;

	auto Lexer = SLexer::Default();
	Lexer.Analyze(Source.data());
	auto Statements = RootParse(GetStatementBlockParser(), move(Lexer.Tokens));

	SCompilingData Data;
	FProgram Program;
	FRoutine Routine(nullptr);
	FScope Scope(nullptr);
	Data.Program = &Program;
	Data.Routine = &Routine;
	Data.Scope = &Scope;

	InitializeBuiltIn(&Scope);

	for (auto Statement : Statements)
		Statement->GenerateCode(Data, nullptr);
	
	for (auto Statement : Statements)
		delete Statement;

	Result.Instructions = Routine.GetInstructions();
	Result.Labels.resize(Program.GetLabelCount());
	Result.Errors = move(*Program.GetErrors());
	Result.Data = move(*Program.GetData());

	auto Startup = Scope.FindProcedure("startup");
	if (!Startup)
		Program.AddError(SLocation(0, 0, 0), "スタートアッププロシージャ'startup'がルートスコープに定義されていません。");
	else
		Result.Instructions.insert(Result.Instructions.begin(), SInstruction(EInstruction::CALL, SType(), Startup->Label, 0, 0));
	
	copy(Result.Errors.begin(), Result.Errors.end(), back_inserter(Lexer.Errors));
	
	uint ProgramCount = 0;
	for (auto& Instruction : Result.Instructions)
	{
		if (Instruction.Operation.Code == EInstruction::LABEL)
			Result.Labels[Instruction.Operand0] = ProgramCount;
		ProgramCount++;
	}

	return Result;
}