#pragma once
#include "MValue.h"

struct SVariableType
{
	bool IsInteger;
	bool IsConstant;
};

class FVariableTypeTable
{
private:
	vector<string> Variables;

public:
	bool Get(string Name) const
	{
		return find(Variables.begin(), Variables.end(), Name) != Variables.end();
	}
};

class FMatchingTable
{
private:
	map<string, unique_ptr<class FExpression>> Variables;

public:
	bool Match(string Name, FTerm const& Term);
	class FExpression* GetValue(string Name) const { return Variables[Name].get(); }
};

struct SMatchingData
{
	FVariableTypeTable DestIntegerTable;
	FVariableTypeTable RuleIntegerTable;
	FVariableTypeTable RuleConstantTable;
	FMatchingTable MatchingTable;
};

enum EExpressionType
{
	Terms, Add, Mul, Div
};

class FExpression
{
private:
	EExpressionType Type;
	vector<class FTerm> Terms;
	FValue Degree;
	vector<shared_ptr<FExpression>> Expressions;

public:
	FExpression() 
		: Type(EExpressionType::Terms) {}
	FExpression(FTerm Term) 
		: Type(EExpressionType::Terms) { Terms.push_back(Term); }
	FExpression(EExpressionType Type, vector<shared_ptr<FExpression>>&& Expressions) 
		: Type(Type), Expressions(move(Expressions)) {}

	FExpression(vector<class FTerm> const& Terms);

	void AddExpression(FExpression Expression);

	bool IsEmpty() const { return Type == EExpressionType::Terms && Terms.size() == 0; }
	bool IsMonomial() const { return Terms.size() == 1; }
	class FTerm GetMonomial() const { return Terms[0]; }
	FExpression Assign(FMatchingTable const& MatchingTable);
	FExpression ApplyRule(FExpression const& Target, SMatchingData* Data);
	void ApplyRule(struct SRule const& Rule);
	bool Equals(FExpression const& Target) const;
	string ToString() const;
};

struct SRule
{
	FExpression Left;
	FExpression Right;
};