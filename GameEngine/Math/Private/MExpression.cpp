#include "..\Public\MExpression.h"
#include "MValue.h"
#include "MMatch.h"

bool FExpression::IsInteger() const
{
	return false;
}

bool FExpression::ApplyRule(FRule const& Rule, FExpression* Result) const
{
	auto Target = Rule.GetBefore();
	FMatchingTable MatchingTable;

	switch (Type)
	{
	case EExpressionType::Add:
	case EExpressionType::Sub:
	case EExpressionType::Mul:
	case EExpressionType::Div:
	{
		if (Type != Target.Type)
			return false;

		auto NoMatches = MatchExpressions(Type, Elements, Target.Elements, MatchingTable);
		if (NoMatches.size() == 0)
			return false;

		*Result = Rule.GetAfter().Assign(MatchingTable);
		uint Index = 0;
		for (auto NoMatche : NoMatches)
		{
			if (!NoMatche.IsEmpty())
				Result->Elements.push_back(NoMatche);
			Index++;
		}
		return true;
	}
	default:
		if (!Match(Target, MatchingTable))
			return false;
		*Result = Rule.GetAfter().Assign(MatchingTable);
		return true;
	}
}

bool FExpression::ApplyRuleThroughElement(FRuleTable& RuleTable, FExpression* Result) const
{
	auto Rules = RuleTable.GetRules(Name);
	for (auto& Rule : Rules)
	{
		if (ApplyRule(Rule, Result))
			return true;
	}

	switch (Type)
	{
	case EExpressionType::Add:
	case EExpressionType::Sub:
	case EExpressionType::Mul:
	case EExpressionType::Div:
	case EExpressionType::Function:
	{
		uint Index = 0;
		for (auto& Element : Elements)
		{
			FExpression ElementResult;
			if (Element.ApplyRuleThroughElement(RuleTable, &ElementResult))
			{
				auto ElementsResult = Elements;
				ElementsResult[Index] = ElementResult;
				*Result = FExpression(Type, Name, ElementsResult);
				return true;
			}
			Index++;
		}
		return false;
	}
	default:
		return false;
	}
}

FManipulation FExpression::Calculate(FRuleTable& RuleTable) const
{
	FManipulation Manipulation(*this);

	auto Normalized = Normalize();
	if (!Equals(Normalized))
		Manipulation.AddProcess(Normalized);

	FExpression Result = Normalized;
	while (true)
	{
		if (Result.ApplyRuleThroughElement(RuleTable, &Result))
		{
			Manipulation.AddProcess(Result);

			auto NormalizedResult = Result.Normalize();
			if (!NormalizedResult.Equals(Result))
			{
				Manipulation.AddProcess(NormalizedResult);
				Result = NormalizedResult;
			}
		}
		else
		{
			break;
		}
	}
	return Manipulation;
}

FExpression FExpression::Assign(FMatchingTable& MatchingTable) const
{
	switch (Type)
	{
	case EExpressionType::Variable:
		if (MatchingTable.HasVariable(Name))
			return MatchingTable.GetVariable(Name);
		else
			return *this;
	case EExpressionType::MathConstant:
	case EExpressionType::Constant:
		return *this;
	case EExpressionType::Add:
	case EExpressionType::Sub:
	case EExpressionType::Mul:
	case EExpressionType::Div:
	case EExpressionType::Pow:
	case EExpressionType::Function:
	{
		vector<FExpression> AssignedElemetns;
		for (auto& Element : Elements)
			AssignedElemetns.push_back(Element.Assign(MatchingTable));
		return FExpression(Type, Name, AssignedElemetns);
	}
	}
}

bool FExpression::Match(FExpression const& Target, FMatchingTable& MatchingTable) const
{
	if (Type == Target.Type)
	{
		switch (Type)
		{
		case EExpressionType::Variable:
			return MatchingTable.MatchVariable(Target.Name, *this);
		case EExpressionType::Function:
			if (Name != Target.Name)
				return false;
			return MatchElementsEachSameIndex(Elements, Target.Elements, MatchingTable);
		case EExpressionType::MathConstant:
			return Name == Target.Name;
		case EExpressionType::Constant:
			return Value == Target.Value;
		case EExpressionType::Pow:
			return MatchElementsEachSameIndex(Elements, Target.Elements, MatchingTable);
		case EExpressionType::Add:
		case EExpressionType::Sub:
		case EExpressionType::Mul:
		case EExpressionType::Div:
			return MatchElementsWithoutIndexMatching(Type, Elements, Target.Elements, MatchingTable);
		}
	}
	else if (Type == EExpressionType::Constant)
	{
		switch (Target.Type)
		{
		case EExpressionType::Variable:
			return MatchingTable.MatchVariable(Target.Name, *this);
		case EExpressionType::Add:
		case EExpressionType::Sub:
		case EExpressionType::Mul:
		case EExpressionType::Div:
			return MatchElementsWithoutIndexMatching(Target.Type, { *this }, Target.Elements, MatchingTable);
		}
	}
	else
	{
		switch (Target.Type)
		{
		case EExpressionType::Add:
		case EExpressionType::Sub:
			return MatchHiddenConstant(true, *this, Target.Elements, MatchingTable);
		case EExpressionType::Mul:
		case EExpressionType::Div:
			return MatchHiddenConstant(false, *this, Target.Elements, MatchingTable);
		}
	}
	
	return false;
}

string FExpression::ToString() const
{
	string Operator = "Error";
	string Result = "";

	switch (Type)
	{
	case EExpressionType::Variable:
		return Name;
	case EExpressionType::MathConstant:
		return Name;
	case EExpressionType::Constant:
		return Value.str();
	case EExpressionType::Add:
		Operator = "+";
		break;
	case EExpressionType::Sub:
		Operator = "-";
		break;
	case EExpressionType::Mul:
		Operator = "*";
		break;
	case EExpressionType::Div:
		Operator = "/";
		break;
	case EExpressionType::Pow:
		Operator = "^";
		break;
	case EExpressionType::Function:
		Operator = ",";
		break;
	}

	for (auto& Element : Elements)
		Result += Operator + Element.ToString();
	Result.erase(Result.begin());

	if (Operator == ",")
		return Name + "(" + Result + ")";
	else
		return "(" + Result + ")";
}

bool FExpression::Equals(FExpression const& Target) const
{
	if (Type != Target.Type)
		return false;

	switch (Type)
	{
	case EExpressionType::Empty:
		return true;
	case EExpressionType::Variable:
	case EExpressionType::MathConstant:
		return Name == Target.Name;
	case EExpressionType::Function:
		if (Name != Target.Name)
			return false;
		for (uint i = 0; i < Elements.size(); i++)
		{
			if (!Elements[i].Equals(Target.Elements[i]))
				return false;
		}
		return true;
	case EExpressionType::Constant:
		return Value == Target.Value;
	case EExpressionType::Add:
	case EExpressionType::Sub:
	case EExpressionType::Mul:
	case EExpressionType::Div:
	case EExpressionType::Pow:
		if (Elements.size() != Target.Elements.size())
			return false;
		for (uint i = 0; i < Elements.size(); i++)
		{
			if (!Elements[i].Equals(Target.Elements[i]))
				return false;
		}
		return true;
	}

	assert(false);
	return false;
}

double FExpression::GetApproximation() const
{
	return ToValue().GetApproximation();
}

bool FExpression::HasValue() const
{
	switch (Type)
	{
	case EExpressionType::NaN:
	case EExpressionType::Function:
	case EExpressionType::Variable:
	case EExpressionType::MathConstant:
		return false;
	case EExpressionType::Constant:
		return true;
	case EExpressionType::Add:
	case EExpressionType::Sub:
	case EExpressionType::Mul:
	case EExpressionType::Div:
	case EExpressionType::Pow:
		for (auto& Element : Elements)
		{
			if (!Element.HasValue())
				return false;
		}
		return true;
	}

	assert(false);
	return false;
}

class FValue FExpression::ToValue() const
{
	if (Type == EExpressionType::Constant)
		return FValue(Value);

	FValue Result(0);

	bool IsFirst = true;
	switch (Type)
	{
	case EExpressionType::Add:
		for (auto& Element : Elements)
			Result.Add(Element.ToValue());
		return Result;
	case EExpressionType::Sub:
		for (auto& Element : Elements)
			Result.Add(Element.ToValue().FlipSign());
		return Result;
	case EExpressionType::Mul:
		for (auto& Element : Elements)
			Result.Mul(Element.ToValue());
		return Result;
	case EExpressionType::Div:
		Result = mint(1);
		for (auto& Element : Elements)
		{
			auto Value = Element.ToValue();
			if (!IsFirst)
				Value = Value.Reciprocal();
			Result.Mul(Value);
			IsFirst = false;
		}
		return Result;
	case EExpressionType::Pow:
		for (auto& Element : Elements)
			Result.Pow(Element.ToValue());
		return Result;
	}
	
	assert(!"check by HasValue");
	return FValue(0);
}

FManyTerms FExpression::ToManyTerms() const
{
	FManyTerms Terms;
	bool IsFirst = true;
	for (auto& Element : Elements)
	{
		bool IsMinus = Type == EExpressionType::Sub && !IsFirst;
		switch (Element.Type)
		{
		case EExpressionType::Add:
		case EExpressionType::Sub:
		{
			auto ElementTerms = Element.ToManyTerms();
			for (auto ElementTerm : ElementTerms.GetTerms())
			{
				if (IsMinus)
					ElementTerm.ConstantPart = ElementTerm.ConstantPart.FlipSign();
				Terms.Add(ElementTerm);
			}
			break;
		}
		default:
		{
			auto Term = Element.NormalizeToTerm();
			if (IsMinus)
				Term.ConstantPart = Term.ConstantPart.FlipSign();
			Terms.Add(Term);
			break;
		}
		IsFirst = false;
		}
	}
	return Terms;
}

FManyParts FExpression::ToManyParts() const
{
	FManyParts Parts;
	bool IsFirst = true;
	for (auto& Element : Elements)
	{
		bool IsReciprocal = Type == EExpressionType::Div && !IsFirst;
		switch (Element.Type)
		{
		case EExpressionType::Mul:
		case EExpressionType::Div:
		{
			auto ElementParts = Element.ToManyParts();
			for (auto ElementPart : ElementParts.GetParts())
			{
				if (IsReciprocal)
					ElementPart.Degree = FExpression(EExpressionType::Sub, { ElementPart.Degree });
				Parts.Mul(ElementPart);
			}
			break;
		}
		case EExpressionType::Pow:
		{
			SPart Part;
			Part.Main = Elements[0];
			Part.Degree = Elements[1];
			Parts.Mul(Part);
			break;
		}
		default:
		{
			auto Term = Element.NormalizeToTerm();
			if (IsReciprocal)
				Term.ConstantPart = Term.ConstantPart.Reciprocal();
			Parts.Mul(Term);
			break;
		}
		}
		IsFirst = false;
	}
	return Parts;
}

STerm FExpression::NormalizeToTerm() const
{
	{
		STerm Result;
		switch (Type)
		{
		case EExpressionType::Empty:
			assert(false);
		case EExpressionType::Function:
		{
			vector<FExpression> NormalizedElements;
			NormalizedElements.reserve(Elements.size());
			for (auto& Element : Elements)
				NormalizedElements.push_back(Element.Normalize());
			Result.MainPart = FExpression(Type, Name, NormalizedElements);
			return Result;
		}
		case EExpressionType::Variable:
			Result.MainPart = *this;
			return Result;
		case EExpressionType::MathConstant:
			Result.MainPart = *this;
			return Result;
		case EExpressionType::Constant:
			Result.ConstantPart = FValue(Value);
			return Result;
		}
	}

	if (Elements.size() == 1)
		return Elements.front().NormalizeToTerm();

	switch (Type)
	{
	case EExpressionType::Add:
	case EExpressionType::Sub:
	{
		return ToManyTerms().ToTerm();
	}
	case EExpressionType::Div:
	{
		if (Elements.size() == 2 && Elements[0].HasValue() && Elements[1].HasValue())
		{
			STerm Term;
			Term.ConstantPart = Elements[0].ToValue();
			Term.ConstantPart.Mul(Elements[1].ToValue().Reciprocal());
			return Term;
		}
		else
		{
			STerm Term;
			Term.MainPart = *this;
			return Term;
		}
	}
	case EExpressionType::Mul:
	{
		return ToManyParts().ToTerm();
	}
	case EExpressionType::Pow:
		if (Elements.size() == 2)
		{
			auto Main = Elements[0].NormalizeToTerm();
			auto Degree = Elements[1].NormalizeToTerm();

			if (Degree.MainPart.IsEmpty() && Degree.ConstantPart.IsOne())
			{
				STerm Result;
				Result.MainPart = Elements[0];
				return Result;
			}
			/*
			if (Degree.MainPart.IsEmpty())
			{
				STerm Term;
				Term.ConstantPart = Main.ConstantPart;
				Term.ConstantPart.Mul(Degree.ConstantPart);
				if (Main.MainPart.Type == EExpressionType::Mul)
				{

				}

				for (auto MainElement : Main.MainPart.Elements)
				{

				}
			}*/
		}
		else
		{
			
		}
		{
			STerm Term;
			Term.MainPart = *this;
			return Term;
		}
	default:
		break;
	}

	////Result.MainPart = *this;
	//return Result;
	
	assert(false);
}

FExpression FExpression::Normalize() const
{
	return NormalizeToTerm().ToExpression();
}

FExpression FExpression::Mul(class FValue Value) const
{
	if (Value.IsOne())
		return *this;

	if (Type == EExpressionType::Mul)
	{
		vector<FExpression> ResultElements;
		ResultElements.reserve(Elements.size() + 1);
		ResultElements.push_back(Value.ToExpression());
		for (auto& Element : Elements)
			ResultElements.push_back(Element);
		return FExpression(EExpressionType::Mul, ResultElements);
	}
	else
	{
		return FExpression(EExpressionType::Mul, { Value.ToExpression(), *this });
	}
}

SPart FExpression::ToPart() const
{
	SPart Part;
	if (Type == EExpressionType::Pow)
	{
		Part.Main = Elements[0];
		if (Elements.size() == 2)
		{
			Part.Degree = Elements[1];
		}
		else
		{
			auto NewElements = Elements;
			NewElements.erase(NewElements.begin());
			Part.Degree = FExpression(EExpressionType::Pow, NewElements);
		}
	}
	else
	{
		Part.Degree = FExpression(1);
		Part.Main = *this;
	}

	return Part;
}

FRuleTable::FRuleTable(vector<FRule> InRules)
{
	for (auto& Rule : InRules)
	{
		if (Rule.GetBefore().IsFunction())
			Rules[Rule.GetBefore().GetName()].emplace_back(Rule);
		else
			Rules[""].emplace_back(Rule);
	}
}