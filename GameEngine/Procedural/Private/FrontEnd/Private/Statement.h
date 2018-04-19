#pragma once
#include "Common.h"

class FCompiler;
class FExpression;
class FStatementDeclaration;

class FStatement
{
public:
	virtual const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) = 0;
};

class FStatementNOP : public FStatement
{
public:
	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override {}
};

class FStatementInvaildToken : public FStatement
{
private:
	SToken Token;

public:
	FStatementInvaildToken(SToken Token) : Token(Token) {}

	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override;
};

class FStatementBlock : public FStatement
{
private:
	vector<FStatement*> Statements;

public:
	FStatementBlock(vector<FStatement*> Statements) : Statements(Statements) {}

	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override;
};

class FStatementDefineFunction : public FStatement
{
private:
	SType ReturnType;
	SToken Name;
	vector<FStatementDeclaration*> Arguments;
	unique_ptr<FStatementBlock> Block;

public:
	FStatementDefineFunction(SType ReturnType, SToken Name, vector<FStatementDeclaration*> Arguments, FStatementBlock* Block)
		: ReturnType(ReturnType), Name(Name), Arguments(Arguments), Block(Block) {}

	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override;
};

class FStatementDefineProcedure : public FStatement
{
private:
	SToken Name;
	SToken TargetShape;
	vector<FStatementDeclaration*> Arguments;
	unique_ptr<FStatementBlock> Block;
	SType ForkYieldType;
	bool IsTagNotValue;

public:
	FStatementDefineProcedure(SToken Name, SToken TargetShape, vector<FStatementDeclaration*> Arguments, SType ForkYieldType, bool IsTagNotValue, FStatementBlock* Block)
		: Name(Name), TargetShape(TargetShape), Arguments(Arguments), Block(Block), ForkYieldType(ForkYieldType), IsTagNotValue(IsTagNotValue) {}

	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override;
};

class FStatementYield : public FStatement
{
private:
	unique_ptr<FStatementBlock> Block;
	SToken Tag;
	unique_ptr<FExpression> Value;
	bool IsTagNotValue;

public:
	FStatementYield(SToken Tag, FStatementBlock* Block) : IsTagNotValue(true), Tag(Tag), Block(Block) {}
	FStatementYield(FExpression* Value, FStatementBlock* Block) : IsTagNotValue(false), Value(Value), Block(Block) {}

	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override;
};

class FStatementDefineShape : public FStatement
{
private:
	SToken Name;
	vector<pair<SType, SToken>> Members;
	vector<FStatementBlock*> Converters;

public:
	FStatementDefineShape(SToken Name, vector<pair<SType, SToken>> Members) : Name(Name), Members(Members) {}

	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override;
};

class FStatementDeclaration : public FStatement
{
private:
	SType Type;
	SToken Name;
	unique_ptr<FExpression> Expression;

public:
	FStatementDeclaration(SType Type, string Name)
		: Type(Type), Name(Name), Expression(nullptr) {}

	FStatementDeclaration(SType Type, SToken Name, FExpression* Expression)
		: Type(Type), Name(Name), Expression(Expression) {}

	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override;
	SType GetType() const { return Type; }
};

class FStatementAssignation : public FStatement
{
private:
	SToken Name;
	SToken Operator;
	unique_ptr<FExpression> Expression;

public:
	FStatementAssignation(SToken Name, SToken Operation, FExpression* Expression)
		: Name(Name), Operator(Operation), Expression(Expression) {}

	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override;
};

struct SStatementIfBlock
{
	unique_ptr<FExpression> Condition;
	unique_ptr<FStatementBlock> Block;
	SLocation IfOrElse;

	SStatementIfBlock(SLocation IfOrElse, FExpression* Condition, FStatementBlock* Block) 
		: IfOrElse(IfOrElse), Condition(Condition), Block(Block) {}
};

class FStatementIf : public FStatement
{
private:
	vector<unique_ptr<SStatementIfBlock>> CaseBlocks;
	unique_ptr<FStatementBlock> ElseBlock;

public:
	FStatementIf(SStatementIfBlock* IfBlock, vector<SStatementIfBlock*> ElseIfBlocks, FStatementBlock* ElseBlock) 
		: ElseBlock(ElseBlock) 
	{
		CaseBlocks.emplace_back(IfBlock);
		for (auto ElseIfBlock : ElseIfBlocks)
			CaseBlocks.emplace_back(ElseIfBlock);
	}

	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override;
};

class FStatementWhile : public FStatement
{
private:
	unique_ptr<FExpression> Expression;
	unique_ptr<FStatementBlock> Block;
	SLocation WhileLocation;

public:
	FStatementWhile(SLocation WhileLocation, FExpression* Expression, FStatementBlock* Block) 
		: WhileLocation(WhileLocation), Expression(Expression), Block(Block) {}

	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override;
};

class FStatementReturn : public FStatement
{
private:
	unique_ptr<FExpression> Expression;

public:
	FStatementReturn(FExpression* Expression) : Expression(Expression) {}

	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override;
};

struct SForkBlock
{
	FExpression* Condition;
	bool IsTagNotValue;
	union
	{
		FStatementBlock* Block;
		SToken Tag;
	};

	SForkBlock() {}
	SForkBlock(FExpression* Condition, FStatementBlock* Block) : IsTagNotValue(false), Condition(Condition), Block(Block) {}
	SForkBlock(string Tag, FStatementBlock* Block) : IsTagNotValue(true), Tag(Tag), Block(Block) {}
};

class FStatementProcedure : public FStatement 
{
private:
	SToken Name;
	vector<FExpression*> Arguments;
	vector<SForkBlock> ForkCases;

public:
	FStatementProcedure(SToken Name, vector<FExpression*> Arguments, vector<SForkBlock> ForkBlocks)
		: Name(Name), Arguments(Arguments), ForkCases(move(ForkBlocks)) {}

	const SShape* GenerateCode(const SCompilingData& Data, const SShape* InputShape) override;
};