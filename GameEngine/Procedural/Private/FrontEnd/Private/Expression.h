#pragma once
#include "Common.h"
#include "Statement.h"	

class FCompiler;

struct SExpressionGenerationResult
{
	SLiteral Literal;
	bool IsLiteralVaild;
	regptr Register;

	SExpressionGenerationResult() {}
	SExpressionGenerationResult(nullptr_t Null) : IsLiteralVaild(false) {}
	SExpressionGenerationResult(const SLiteral& Literal) : Literal(Literal), IsLiteralVaild(true) {}
	SExpressionGenerationResult(regptr Register) : Register(Register), IsLiteralVaild(false) {}
};

class FExpression
{
protected:
	SType Type;

public:
	FExpression() {}
	FExpression(SType Type) : Type(Type) {}

	SType GetType() const { return Type; }
	regptr GenerateCode(const SCompilingData& Data, regptr Register);
	void GenerateCodeOrMove(const SCompilingData& Data, regptr Register);

	virtual ~FExpression() {}	
	virtual SExpressionGenerationResult GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register) = 0;
	virtual SLocation GetLocation() const = 0;
};

class FExpressionLiteral : public FExpression
{
private:
	SToken Value;

public:
	FExpressionLiteral(SToken Value, SType Type) : FExpression(Type), Value(Value) {}
	~FExpressionLiteral() override {}

	SExpressionGenerationResult GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register) override;
	SLocation GetLocation() const override { return Value.Location; }
};

class FExpressionVariable : public FExpression
{
private:
	SToken Name;

public:
	FExpressionVariable(SToken Name) : Name(Name) {}
	~FExpressionVariable() override {}

	SExpressionGenerationResult GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register) override;
	SLocation GetLocation() const override { return Name.Location; }
};

class FExpressionOperation : public FExpression
{
private:
	unique_ptr<FExpression> Left;
	unique_ptr<FExpression> Right;
	SToken Operator;

public:
	FExpressionOperation(FExpression* Left, SToken Operation, FExpression* Right) : Left(Left), Operator(Operation), Right(Right) {}

	SExpressionGenerationResult GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register) override;
	SLocation GetLocation() const override { return SLocation::Between(Left->GetLocation(), Right->GetLocation()); }
};

class FExpressionUnaryOperation : FExpression
{
private:
	unique_ptr<FExpression> Unary;
	SToken Operator;

public:
	FExpressionUnaryOperation(SToken Operation, FExpression* Unary) : Operator(Operation), Unary(Unary) {}

	SExpressionGenerationResult GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register) override;
	SLocation GetLocation() const override { return SLocation::Between(Operator.Location, Unary->GetLocation()); }
};

class FExpressionFunctionCall : public FExpression
{
private:
	SToken Name;
	vector<FExpression*> Arguments;

public:
	FExpressionFunctionCall(SToken Name, vector<FExpression*> Arguments) : Name(Name), Arguments(Arguments) {}

	SExpressionGenerationResult GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register) override;
	SLocation GetLocation() const override { return Name.Location; }
};

class FExpressionAccessor : public FExpression
{
private:
	unique_ptr<FExpression> Target;
	SToken Member;

public:
	FExpressionAccessor(FExpression* Expression, SToken Member) : Target(Expression), Member(Member) {}

	SExpressionGenerationResult GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register) override;
	SLocation GetLocation() const override { return SLocation::Between(Target->GetLocation(), Member.Location); }
};

class FExpressionConvert : public FExpression
{
private:
	unique_ptr<FExpression> Target;
	EValueType DestValueType;

public:
	FExpressionConvert(unique_ptr<FExpression>&& Target, EValueType DestValueType) : Target(move(Target)), DestValueType(DestValueType) {}
	FExpressionConvert(FExpression* Target, EValueType DestValueType) : Target(Target), DestValueType(DestValueType) {}

	SExpressionGenerationResult GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register) override;
	SLocation GetLocation() const override { return Target->GetLocation(); }
};