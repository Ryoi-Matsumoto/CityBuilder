#pragma once
#include "MValue.h"
#include "MExpression.h"

class FVariablePart
{
	friend class FTerm;

private:
	FValue Degree;
	string Name;
	vector<FExpression> Arguments;
	bool IsFunction;

public:
	FVariablePart(string VariableName)
		: Degree(1)
		, Name(VariableName)
		, IsFunction(false) {}

	FVariablePart(string FunctionName, vector<FExpression> Arguments)
		: Degree(1)
		, Name(FunctionName)
		, Arguments(Arguments)
		, IsFunction(true) {}

	string GetVariableName() const;
	bool GetIsFunction() const;
	void Pow(FValue Value);
	bool Add(FVariablePart const& Target);
	bool Equals(FVariablePart const& Target) const;
	string ToString() const;
};

class FTerm
{
private:
	FValue Coefficient;
	vector<FVariablePart> Parts;

public:
	FTerm() : Coefficient(1) {}

	FTerm(int Constant) : Coefficient(FValue(Constant)) {}

	FTerm(FValue Constant) : Coefficient(Constant) {}

	FTerm(string VariableName)
	{
		Parts.emplace_back(VariableName);
	}

	FTerm(string FunctionName, vector<FExpression> Arguments)
	{
		Parts.emplace_back(FunctionName, Arguments);
	}

	FValue GetConstant() const;
	bool IsZero() const;
	void Pow(FValue Value);
	void FlipSign();
	bool IsConstant() const;
	bool AddTerm(FTerm const& Term);
	void MulTerm(FTerm const& Term);
	bool Equals(FTerm const& Target) const;
	string ToString() const;
	bool Match(FTerm const& Target, SMatchingData* Data) const;
	FExpression Assign(FMatchingTable const& MatchingTable) const;
};