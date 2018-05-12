#pragma once

enum class EIntermediateType
{
	Add, Sub, Mul, Div, Pow, Value, Variable, Function, Minus
};

class SIntermediate
{
private:
	EIntermediateType Type;
	unique_ptr<SIntermediate> Left;
	unique_ptr<SIntermediate> Right;
	int Value;
	string Variable;
	string Function;
	vector<SIntermediate*> Arguments;

private:
	vector<class FTerm> CalculateTerms() const;

public:
	SIntermediate() {}

	SIntermediate(int Value) : Type(EIntermediateType::Value), Value(Value) {}

	SIntermediate(const char* Variable) : Type(EIntermediateType::Variable), Variable(Variable) {}

	SIntermediate(const char* Function, vector<SIntermediate*> Arguments)
		: Type(EIntermediateType::Function)
		, Function(Function)
		, Arguments(Arguments) {}

	SIntermediate(EIntermediateType Type, SIntermediate* Left, SIntermediate* Right)
		: Type(Type), Left(Left), Right(Right) {}

	SIntermediate(EIntermediateType Type, SIntermediate* Left)
		: Type(Type), Left(Left) {}

	class FExpression Calculate() const;
};