#pragma once
#include "Common.h"
#include "Statement.h"	

class FCompiler;

class FExpression
{
	friend class FExpressionLiteral;
	friend class FExpressionVariable;
	friend class FExpressionOperation;
	friend class FExpressionUnaryOperation;
	friend class FExpressionFunctionCall;
	friend class FExpressionAccessor;

private:
	bool IsResolvedSymbols;

protected:
	EValueType Type;

private:
	// OnlyGenerateCodeWithoutLiteral + OnlyGetLiteral = OnlyGenerateCode
	// ResolveSymbols + OnlyGenerateCode = GenerateCode
	// ResolveSymbols + OnlyGetLiteral = GetLiteral
	// GenerateCode + Move = GenerateCodeOrMove
	virtual void ResolveSymbols(const SCompilingData& Data) = 0;
	virtual regptr OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const = 0;
	virtual SLiteral OnlyGetLiteral() const = 0;
	regptr OnlyGenerateCode(const SCompilingData& Data, regptr Register);

public:
	FExpression() : Type(EValueType::Error), IsResolvedSymbols(false) {}
	FExpression(EValueType Type) : Type(Type), IsResolvedSymbols(false) {}

	EValueType GetType() const { assert(Type != EValueType::Error); return Type; }
	regptr GenerateCode(const SCompilingData& Data, regptr Register);
	void GenerateCodeOrMove(const SCompilingData& Data, regptr Register);
	SLiteral GetLiteral(const SCompilingData& Data);

	virtual ~FExpression() {}
	virtual SLocation GetLocation() const = 0;
};

class FExpressionLiteral : public FExpression
{
private:
	SToken Value;
	SLiteral Literal;

private:
	void ResolveSymbols(const SCompilingData& Data) override;
	regptr OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const override;
	SLiteral OnlyGetLiteral() const override;

public:
	FExpressionLiteral(SToken Value, EValueType Type) : FExpression(Type), Value(Value) {}
	~FExpressionLiteral() override {}

	SLocation GetLocation() const override { return Value.Location; }
};

class FExpressionVariable : public FExpression
{
private:
	SToken Name;
	const SVariable* Variable;

private:
	void ResolveSymbols(const SCompilingData& Data) override;
	regptr OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const override;
	SLiteral OnlyGetLiteral() const override;

public:
	FExpressionVariable(SToken Name) : Name(Name) {}
	~FExpressionVariable() override {}

	SLocation GetLocation() const override { return Name.Location; }
};

class FExpressionOperation : public FExpression
{
private:
	unique_ptr<FExpression> Left;
	unique_ptr<FExpression> Right;
	SToken Operator;
	EInstruction OperationCode;
	EOperationType OperationType;

private:
	void ResolveSymbols(const SCompilingData& Data) override;
	regptr OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const override;
	SLiteral OnlyGetLiteral() const override;

public:
	FExpressionOperation(FExpression* Left, SToken Operation, FExpression* Right) : Left(Left), Operator(Operation), Right(Right) {}
	~FExpressionOperation() override {}

	SLocation GetLocation() const override { return SLocation::Between(Left->GetLocation(), Right->GetLocation()); }
};

class FExpressionUnaryOperation : FExpression
{
private:
	unique_ptr<FExpression> Unary;
	SToken Operator;
	EInstruction OperationCode;
	EOperationType OperationType;

private:
	void ResolveSymbols(const SCompilingData& Data) override;
	regptr OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const override;
	SLiteral OnlyGetLiteral() const override;

public:
	FExpressionUnaryOperation(SToken Operation, FExpression* Unary) : Operator(Operation), Unary(Unary) {}
	~FExpressionUnaryOperation() override {}

	SLocation GetLocation() const override { return SLocation::Between(Operator.Location, Unary->GetLocation()); }
};

class FExpressionFunctionCall : public FExpression
{
private:
	SToken Name;
	vector<FExpression*> Arguments;
	const SFunction* Function;

private:
	void ResolveSymbols(const SCompilingData& Data) override;
	regptr OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const override;
	SLiteral OnlyGetLiteral() const override;

public:
	FExpressionFunctionCall(SToken Name, vector<FExpression*> Arguments) : Name(Name), Arguments(Arguments) {}
	~FExpressionFunctionCall() override {}

	SLocation GetLocation() const override { return Name.Location; }
};

class FExpressionAccessor : public FExpression
{
private:
	unique_ptr<FExpression> Expression;
	SToken Member;
	int MemberIndex;
	EOperationType OperationType;

private:
	void ResolveSymbols(const SCompilingData& Data) override;
	regptr OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const override;
	SLiteral OnlyGetLiteral() const override;

public:
	FExpressionAccessor(FExpression* Expression, SToken Member) : Expression(Expression), Member(Member) {}

	SLocation GetLocation() const override { return SLocation::Between(Expression->GetLocation(), Member.Location); }
};