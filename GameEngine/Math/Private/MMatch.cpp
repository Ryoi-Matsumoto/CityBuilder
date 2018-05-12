#include "MMatch.h"

bool MatchValue(
	EExpressionType Type,
	FValue* Source,
	FValue const& Target)
{
	switch (Type)
	{
	case EExpressionType::Add:
		Source->Add(Target.FlipSign());
		break;
	case EExpressionType::Sub:
		Source->Add(Target);
		break;
	case EExpressionType::Mul:
		Source->Mul(Target.Reciprocal());
		break;
	case EExpressionType::Div:
		Source->Mul(Target);
		break;
	case EExpressionType::Pow:
		break;
	default:
		assert(false);
	}
	return !Source->IsFraction();
}

bool MatchEqual(EExpressionType Type, vector<FExpression>& Source, FExpression const& Target)
{
	vector<FExpression> MatchedElements;
	if (Target.Type == Type)
		MatchedElements = Target.Elements;
	else
		MatchedElements.push_back(Target);

	uint MatchCount = 0;
	for (auto& SourceElement : Source)
	{
		if (SourceElement.IsEmpty())
			continue;

		for (auto& MatchedElement : MatchedElements)
		{
			if (MatchedElement.IsEmpty())
				continue;

			if (SourceElement.HasValue() && MatchedElement.HasValue())
			{
				auto Value = SourceElement.ToValue();
				if (!MatchValue(Type, &Value, MatchedElement.ToValue()))
					break;

				MatchCount++;
				MatchedElement.Clear();
				SourceElement = Value.ToExpression();
				break;
			}
			else if (SourceElement.Equals(MatchedElement))
			{
				MatchCount++;
				MatchedElement.Clear();
				SourceElement.Clear();
				break;
			}
		}

		if (MatchCount == MatchedElements.size())
			return MatchCount;
	}

	return MatchCount;
}

int MatchExpression(
	EExpressionType Type,
	vector<FExpression>& Source,
	FExpression const& Target,
	uint MaxMatchCount,
	FMatchingTable& MatchingTable)
{
	if (Target.IsVariable())
	{
		if (MatchingTable.HasVariable(Target.GetName()))
		{
			// all matching principle
			auto Variable = MatchingTable.GetVariable(Target.GetName());
			return MatchEqual(Type, Source, Variable);
		}
		else
		{
			// maximum matching principle
			vector<FExpression> Matched;
			for (auto& SourceElement : Source)
			{
				if (!SourceElement.IsEmpty() /*&& Source[i]->Match(Target, MatchingTable)*/)
				{
					Matched.push_back(SourceElement);
					SourceElement.Clear();
					if (Matched.size() == MaxMatchCount)
						break;
				}
			}

			if (Matched.size() > 0)
			{
				auto Expression = Matched.size() == 1 ? Matched[0] : FExpression(Type, Matched);
				MatchingTable.SetVariable(Target.GetName(), Expression);
			}

			return Matched.size();
		}
	}
	else
	{
		for (auto& SourceElement : Source)
		{
			if (SourceElement.IsEmpty())
				continue;

			if (SourceElement.HasValue() && Target.HasValue())
			{
				auto Value = SourceElement.ToValue();
				if (MatchValue(Type, &Value, Target.ToValue()))
				{
					SourceElement = Value.ToExpression();
					return 1;
				}
			}
			else
			{
				if (SourceElement.Match(Target, MatchingTable))
				{
					SourceElement.Clear();
					return 1;
				}
			}
		}
	}

	return 0;
}

vector<FExpression> MatchExpressions(
	EExpressionType Type,
	vector<FExpression> const& Source,
	vector<FExpression> const& Targets,
	FMatchingTable& MatchingTable)
{
	auto MatchingSource = Source;

	uint TotalMatchCount = 0;
	uint Index = 0;
	for (auto& Target : Targets)
	{
		auto MatchableCount = Source.size() - TotalMatchCount;
		auto RestTargetsCount = Targets.size() - Index - 1;
		auto MaxMatchCount = MatchableCount - RestTargetsCount;

		auto MatchCount = MatchExpression(Type, MatchingSource, Target, MaxMatchCount, MatchingTable);
		TotalMatchCount += MatchCount;

		if (MatchCount == 0)
			return vector<FExpression>();

		Index++;
	}

	return MatchingSource;
}

bool MatchElementsEachSameIndex(
	vector<FExpression> const& SourceElements,
	vector<FExpression> const& TargetElements,
	FMatchingTable& MatchingTable)
{
	auto Backup = MatchingTable;
	for (uint i = 0; i < SourceElements.size(); i++)
	{
		if (!SourceElements[i].Match(TargetElements[i], MatchingTable))
		{
			MatchingTable = Backup;
			return false;
		}
	}
	return true;
}

bool MatchElementsWithoutIndexMatching(
	EExpressionType Type,
	vector<FExpression> const& SourceElements,
	vector<FExpression> const& TargetElements,
	FMatchingTable& MatchingTable)
{
	auto Backup = MatchingTable;
	auto NoMatches = MatchExpressions(Type, SourceElements, TargetElements, MatchingTable);

	bool IsSuccessful = true;
	if (NoMatches.size() == 0)
	{
		IsSuccessful = false;
	}
	else
	{
		for (auto NoMatche : NoMatches)
		{
			if (NoMatche.IsEmpty())
				continue;

			switch (Type)
			{
			case EExpressionType::Sub:
			case EExpressionType::Mul:
				if (NoMatche.HasValue() && NoMatche.GetValue() == 0)
					continue;
			case EExpressionType::Div:
			case EExpressionType::Pow:
				if (NoMatche.HasValue() && NoMatche.GetValue() == 1)
					continue;
			}

			IsSuccessful = false;
			break;
		}
	}

	if (!IsSuccessful)
		MatchingTable = Backup;

	return IsSuccessful;
}

bool MatchHiddenConstant(
	bool IsZeroNotOne,
	FExpression const& Source,
	vector<FExpression> const& TargetElements,
	FMatchingTable& MatchingTable)
{
	auto Backup = MatchingTable;
	uint HiddenMatchCount = 0;
	bool IsSuccessful = true;
	for (auto& TargetElement : TargetElements)
	{
		if (TargetElement.IsVariable())
		{
			int HiddenValue = IsZeroNotOne ? 0 : 1;
			if (!MatchingTable.MatchVariable(TargetElement.GetName(), FExpression(HiddenValue)))
			{
				IsSuccessful = false;
				break;
			}
			HiddenMatchCount++;
		}
		else
		{
			if (!Source.Match(TargetElement, MatchingTable))
			{
				IsSuccessful = false;
				break;
			}
		}
	}

	auto NormalMatchCount = TargetElements.size() - HiddenMatchCount;
	if (NormalMatchCount > 1)
		IsSuccessful = false;

	if (!IsSuccessful)
		MatchingTable = Backup;

	return IsSuccessful;
}