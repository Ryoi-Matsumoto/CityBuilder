#include "MExpression.h"
#include "MTerm.h"

bool FMatchingTable::Match(string Variable, FTerm const& Term)
{
	if (Variables.find(Variable) != Variables.end())
	{
		return Variables[Variable].Equals(Term);
	}
	else
	{
		Variables[Variable] = Term;
		return true;
	}
}

FExpression::FExpression(vector<FTerm> const& InTerms) : Type(EExpressionType::Terms)
{
	for (auto& Term : InTerms)
	{
		if (!Term.IsZero())
			Terms.push_back(Term);
	}
}

void FExpression::ApplyRule(SRule const& Rule)
{
	// macth each term
	// assign variable
	for (auto Term : Terms)
	{

	}
}

bool FExpression::Equals(FExpression const& Target) const
{
	if (Terms.size() != Target.Terms.size())
		return false;

	for (uint i = 0; i < Terms.size(); i++)
	{
		if (!Terms[i].Equals(Target.Terms[i]))
			return false;
	}

	return true;
}

string FExpression::ToString() const
{
	if (Terms.size() == 0)
		return "0";
	string Result = Terms[0].ToString();
	for (uint i = 1; i < Terms.size(); i++)
		Result += " + " + Terms[i].ToString();
	return Result;
}

void FExpression::AddExpression(FExpression Expression)
{

}

int MatchTermsAndRuleTerm(vector<const FTerm*>& MatchedTerms, FTerm const& RuleTerm, SMatchingData* Data)
{
	for (uint i = 0; i < MatchedTerms.size(); i++)
	{
		if (MatchedTerms[i] && MatchedTerms[i]->Match(RuleTerm, Data))
		{
			MatchedTerms[i] = nullptr;
			return i;
		}
	}
	return -1;
}

vector<int> MatchTermsAndRuleTerms(vector<FTerm> const& SourceTerms, vector<FTerm> const& RuleTerms, SMatchingData* Data)
{
	vector<const FTerm*> MatchingTerms;
	for (auto& SourceTerm : SourceTerms)
		MatchingTerms.push_back(&SourceTerm);

	vector<int> MatchedFlags(SourceTerms.size());
	memset(&MatchedFlags[0], 0, sizeof(int) * SourceTerms.size());
	for (auto& RuleTerm : RuleTerms)
	{
		auto MatchedIndex = MatchTermsAndRuleTerm(MatchingTerms, RuleTerm, Data);
		if(MatchedIndex < 0)
			return vector<int>();
		MatchedFlags[MatchedIndex] = true;
	}

	return MatchedFlags;
}

FExpression FExpression::Assign(FMatchingTable const& MatchingTable)
{
	if (Type == EExpressionType::Terms)
	{
		FExpression Result;
		for (auto Term : Terms)
			Result.AddExpression(Term.Assign(MatchingTable));
		return Result;
	}
	else
	{
		vector<shared_ptr<FExpression>> AssignedExpressions;
		for (auto Expression : Expressions)
			AssignedExpressions.emplace_back(Expression->Assign(MatchingTable));
		return FExpression(Type, move(AssignedExpressions));
	}
}

FExpression FExpression::ApplyRule(FExpression const& RuleExpression, SMatchingData* Data)
{
	if (Type == EExpressionType::Terms)
	{
		auto Backup = Data->MatchingTable;

		auto MatchedIndex = MatchTermsAndRuleTerms(Terms, RuleExpression.Terms, Data);
		if (MatchedIndex.size() == 0)
		{
			Data->MatchingTable = Backup;
			return FExpression();
		}

		FExpression Result;
		for (auto RuleTerm : RuleExpression.Terms)
			Result.AddExpression(RuleTerm.Assign(Data->MatchingTable));

		for (uint i = 0; i < Terms.size(); i++)
		{
			if (!MatchedIndex[i])
				Result.AddExpression(FExpression(Terms[i]));
		}

		return Result;
	}
	else
	{
		
	}
}