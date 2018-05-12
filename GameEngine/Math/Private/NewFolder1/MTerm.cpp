#include "MTerm.h"

string FVariablePart::GetVariableName() const
{
	return Name;
}

bool FVariablePart::GetIsFunction() const
{
	return IsFunction;
}

void FVariablePart::Pow(FValue Value)
{
	Degree *= Value;
}

bool FVariablePart::Add(FVariablePart const& Target)
{
	bool IsAddable = Target.Name == Name;
	if (IsAddable)
		Degree += Target.Degree;
	return IsAddable;
}

bool FVariablePart::Equals(FVariablePart const& Target) const
{
	if (!Degree.Equals(Target.Degree))
		return false;
	if (Name != Target.Name)
		return false;
	if (IsFunction)
	{
		if (Arguments.size() != Target.Arguments.size())
			return false;
		for (uint i = 0; i < Arguments.size(); i++)
		{
			if (!Arguments[i].Equals(Target.Arguments[i]))
				return false;
		}
	}
	return true;
}

string FVariablePart::ToString() const
{
	string Result = Name;
	if (IsFunction)
	{
		Result += "(";
		for (auto& Argument : Arguments)
			Result += Argument.ToString();
		Result += ")";
	}

	if (Degree.IsFraction())
		Result += "^(" + Degree.ToString() + ")";
	else if (!Degree.IsOne())
		Result += "^" + Degree.ToString();

	return Result;
}

FValue FTerm::GetConstant() const
{
	assert(IsConstant());
	return Coefficient;
}

bool FTerm::IsZero() const
{
	return Coefficient.IsZero();
}

void FTerm::Pow(FValue Value)
{
	Coefficient.Pow(Value);
	for (auto& Part : Parts)
		Part.Pow(Value);
}

void FTerm::FlipSign()
{
	Coefficient.FlipSign();
}

bool FTerm::IsConstant() const
{
	return Parts.size() == 0;
}

bool FTerm::AddTerm(FTerm const& Term)
{
	if (Parts.size() != Term.Parts.size())
		return false;

	for (auto& TargetPart : Term.Parts)
	{
		bool Flag = false;
		for (auto& Part : Parts)
		{
			if (TargetPart.Equals(Part))
			{
				Flag = true;
				break;
			}
		}
		if (!Flag)
			return false;
	}

	Coefficient += Term.Coefficient;
	return true;
}

void FTerm::MulTerm(FTerm const& Term)
{
	Coefficient *= Term.Coefficient;
	for (auto& AddPart : Term.Parts)
	{
		bool Flag = false;
		for (auto& Part : Parts)
		{
			if (Part.Add(AddPart))
			{
				Flag = true;
				break;
			}
		}
		if (!Flag)
			Parts.push_back(AddPart);
	}
}

bool FTerm::Equals(FTerm const& Target) const
{
	if (!Coefficient.Equals(Target.Coefficient))
		return false;
	if (Parts.size() != Target.Parts.size())
		return false;
	for (uint i = 0; i<Parts.size(); i++)
	{
		if (!Parts[i].Equals(Target.Parts[i]))
			return false;
	}
	return true;
}

string FTerm::ToString() const
{
	string Result = "";
	if (!Coefficient.IsOne() || Parts.size() == 0)
		Result = Coefficient.ToString();
	for (auto& Part : Parts)
	{
		if (Result.size() > 0)
			Result += "*";
		Result += Part.ToString();
	}
	return Result;
}

bool FTerm::Match(FTerm const& Target, SMatchingData* Data) const
{
	auto Backup = Data->MatchingTable;

	auto CoeMatch = Coefficient;
	CoeMatch *= Target.Coefficient.Reciprocal();

	vector<bool> MatchedIndexes(Parts.size());
	memset(&MatchedIndexes[0], 0, sizeof(bool) * MatchedIndexes.size());

	bool IsCoeNotOne = !CoeMatch.IsOne();
	bool IsCoeInteger = !CoeMatch.IsFraction();

	vector<vector<FVariablePart>> MatchedParts;
	for (uint j = 0; j < Target.Parts.size(); j++)
	{
		auto& TargetPart = Target.Parts[j];

		bool IsFunction = TargetPart.GetIsFunction();
		bool IsInteger = Data->DestIntegerTable.Get(TargetPart.GetVariableName());
		bool IsConstant = Data->RuleConstantTable.Get(TargetPart.GetVariableName());

		for (uint i = 0; i < Parts.size(); i++)
		{
			auto& Part = Parts[i];
			if (!MatchedIndexes[i])
			{

			}
			else if (IsFunction)
			{
				auto Backup = Data->MatchingTable;

				bool IsMatched = true;
				if (Part.Arguments.size() != TargetPart.Arguments.size())
					IsMatched = false;
				if (IsMatched)
				{
					for (uint i = 0; i < Part.Arguments.size(); i++)
					{
						if (!Part.Arguments[i].Match(TargetPart.Arguments[i], Data))
						{
							IsMatched = false;
							break;
						}
					}
				}

				MatchedIndexes[i] = IsMatched;
			}
			else if (IsInteger)
			{
				if (Data->RuleIntegerTable.Get(Part.Name))
				{
					if (Part.Degree.Equals(TargetPart.Degree))
					{
						MatchedParts[j].push_back(Part);
						MatchedIndexes[i] = true;
					}
				}
			}
			else if (IsConstant)
			{
				if (TargetPart.Name == Part.Name && Part.Degree.Equals(TargetPart.Degree))
				{
					MatchedParts[j].push_back(Part);
					MatchedIndexes[i] = true;
				}
			}
			else
			{
				if (Part.Degree.Equals(TargetPart.Degree))
				{
					MatchedParts[j].push_back(Part);
					MatchedIndexes[i] = true;
				}
			}
		}

		if (MatchedParts[j].size() == 0)
		{
			Data->MatchingTable = Backup;
			return false;
		}
	}

	bool IsCoeMatched = CoeMatch.IsOne();
	for (uint i = 0; i < Target.Parts.size(); i++)
	{
		FTerm Term;

		if (!IsCoeMatched)
		{
			bool IsInteger = Data->DestIntegerTable.Get(Target.Parts[i].Name);
			if (IsInteger) 
			{
				if (!CoeMatch.IsFraction())
				{
					Term.Coefficient = CoeMatch;
					IsCoeMatched = true;
				}
			}
			else
			{
				Term.Coefficient = CoeMatch;
				IsCoeMatched = true;
			}
		}

		Term.Parts = MatchedParts[i];
		if (!Data->MatchingTable.Match(Target.Parts[i].Name, Term))
		{
			Data->MatchingTable = Backup;
			return false;
		}
	}

	if (!IsCoeMatched)
	{
		Data->MatchingTable = Backup;
		return false;
	}

	return true;
}

FExpression FTerm::Assign(FMatchingTable const& MatchingTable) const
{
	FTerm AssignedTerm;
	AssignedTerm.Coefficient = Coefficient;

	vector<shared_ptr<FExpression>> Expressions;

	for (auto Part : Parts)
	{
		auto AssignedPart = MatchingTable.GetValue(Part.Name);
		if (AssignedPart->IsMonomial())
		{
			auto Monomial = AssignedPart->GetMonomial();
			AssignedTerm.Coefficient.MulValue(Monomial.Coefficient);
			for (auto MonomialPart : Monomial.Parts)
				AssignedTerm.Parts.push_back(MonomialPart);
		}
		else
		{
			Expressions.push_back(make_shared<FExpression>(AssignedPart));
		}
	}

	if (Expressions.size() > 0)
	{
		Expressions.insert(Expressions.begin(), make_shared<FExpression>(AssignedTerm));
	}

	return FExpression(EExpressionType::Mul, move(Expressions));
}