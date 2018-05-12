#pragma once
#include "boost\multiprecision\cpp_int.hpp"
using mint = boost::multiprecision::checked_int1024_t;
using muint = boost::multiprecision::checked_uint1024_t;

enum class EExpressionType
{
	Empty, NaN, Function, Variable, MathConstant, Constant, Add, Sub, Mul, Div, Pow
};

class FExpression
{
public:
	EExpressionType Type;
	vector<FExpression> Elements;
	string Name;
	mint Value;

public:
	bool ApplyRule(class FRule const&  Rule, FExpression* Result) const;
	bool ApplyRuleThroughElement(class FRuleTable& RuleTable, FExpression* Result) const;
	class FValue ToValue() const;
	class STerm NormalizeToTerm() const;

public:
	FExpression(string Text);
	FExpression() : Type(EExpressionType::Empty) {}
	FExpression(mint Value) : Type(EExpressionType::Constant), Name(""), Value(Value) {}
	FExpression(EExpressionType Type) : Type(Type) {}
	FExpression(EExpressionType Type, string Name) : Type(Type), Name(Name) {}
	FExpression(EExpressionType Type, string Name, vector<FExpression> Elements) : Type(Type), Name(Name), Elements(Elements) {}
	FExpression(EExpressionType Type, vector<FExpression> Elements) : Type(Type), Name(""), Elements(Elements) {}

	void Clear() { Type = EExpressionType::Empty; }

	mint GetValue() const { return Value; }
	vector<FExpression> const& GetElements() const { return Elements; }
	double GetApproximation() const;
	bool HasValue() const;
	bool IsEmpty() const { return Type == EExpressionType::Empty; }
	bool IsInteger() const;
	bool IsConstant() const { return Type == EExpressionType::Constant; }
	bool IsVariable() const { return Type == EExpressionType::Variable; }
	bool IsFunction() const { return Type == EExpressionType::Function; }
	string const& GetName() const { return Name; }
	EExpressionType GetType() const { return Type; }
	class FManipulation Calculate(class FRuleTable&  RuleTable) const;

	bool Match(FExpression const& Target, class FMatchingTable& MatchingTable) const;
	bool Equals(FExpression const& Target) const;
	FExpression Normalize() const;
	FExpression Assign(class FMatchingTable& MatchingTable) const;
	FExpression Mul(class FValue Value) const;
	class SPart ToPart() const;
	class FManyParts ToManyParts() const;
	class FManyTerms ToManyTerms() const;

	string ToString() const;
};

class FMatchingTable
{
private:
	map<string, FExpression> Variables;

public:
	void SetVariable(string Name, FExpression Value) { Variables[Name] = Value; }
	bool HasVariable(string Name) { return Variables.find(Name) != Variables.end(); }
	FExpression GetVariable(string Name) { return Variables[Name]; }
	bool MatchVariable(string Name, FExpression Value)
	{
		auto Itr = Variables.find(Name);
		if (Itr != Variables.end())
		{
			return Itr->second.Equals(Variables[Name]);
		}
		else
		{
			Variables[Name] = Value;
			return true;
		}
	}
};

class FRule
{
private:
	FExpression Before;
	FExpression After;

public:
	FRule() {}
	FRule(FExpression Before, FExpression After) : Before(Before), After(After) {}

	FExpression GetBefore() const { return Before; }
	FExpression GetAfter() const { return After; }
};

class FRuleTable
{
private:
	map<string, vector<FRule>> Rules;

public:
	FRuleTable() {}
	FRuleTable(string Text);
	FRuleTable(vector<FRule> Rules);

	vector<FRule>& GetRules(string FunctionName) { return Rules[FunctionName]; }
};

struct SProcess
{
	FExpression Expression;
	FRule Rule;
	bool IsNormalizetion;
};

class FManipulation
{
private:
	FExpression Expression;
	vector<SProcess> Processes;

public:
	FManipulation() {}
	FManipulation(FExpression Expression) : Expression(Expression) {}

	void AddProcess(FExpression Expression, FRule Rule)
	{
		SProcess Process;
		Process.Expression = Expression;
		Process.Rule = Rule;
		Process.IsNormalizetion = false;
		Processes.push_back(move(Process));
	}

	void AddProcess(FExpression Expression)
	{
		SProcess Process;
		Process.Expression = Expression;
		Process.IsNormalizetion = true;
		Processes.push_back(move(Process));
	}

	const FExpression& GetExpression() const { return Expression; }
	const vector<SProcess>& GetProcesses() const { return Processes; }
};