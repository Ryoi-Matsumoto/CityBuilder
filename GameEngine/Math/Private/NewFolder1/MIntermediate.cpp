#include "MIntermediate.h"
#include "MExpression.h"
#include "MTerm.h"

vector<FTerm> SIntermediate::CalculateTerms() const
{
	vector<FTerm> Terms;
	auto LeftTerms = Left ? Left->CalculateTerms() : vector<FTerm>();
	auto RightTerms = Right ? Right->CalculateTerms() : vector<FTerm>();

	switch (Type)
	{
	case EIntermediateType::Sub:
		for (auto& Term : RightTerms)
			Term.FlipSign();
	case EIntermediateType::Add:
		for (auto& RightTerm : RightTerms)
		{
			bool Flag = false;
			for (auto& LeftTerm : LeftTerms)
			{
				if (LeftTerm.AddTerm(RightTerm))
				{
					Flag = true;
					break;
				}
			}
			if (!Flag)
				LeftTerms.push_back(RightTerm);
		}
		break;
	case EIntermediateType::Mul:
		for (auto& LeftTerm : LeftTerms)
		{
			for (auto& RightTerm : RightTerms)
			{
				FTerm Term = LeftTerm;
				Term.MulTerm(RightTerm);
				Terms.push_back(Term);
			}
		}
		LeftTerms = Terms;
		break;
	case EIntermediateType::Div:
		if (RightTerms.size() == 1 && RightTerms[0].IsConstant())
		{
			for (auto& LeftTerm : LeftTerms)
			{
				//FTerm Term = LeftTerm;
				LeftTerm.MulTerm(RightTerms[0].GetConstant().Reciprocal());
				//Terms.push_back(Term);
			}
		}
		else assert(false);
		//LeftTerms = Terms;
		break;
	case EIntermediateType::Pow:
		if (LeftTerms.size() == 1 && RightTerms.size() == 1 && RightTerms[0].IsConstant())
		{
			LeftTerms[0].Pow(RightTerms[0].GetConstant());
		}
		else assert(false);
		break;
	case EIntermediateType::Minus:
		for (auto& Term : LeftTerms)
			Term.FlipSign();
		break;
	case EIntermediateType::Value:
		LeftTerms.emplace_back(Value);
		break;
	case EIntermediateType::Variable:
		LeftTerms.emplace_back(Variable);
		break;
	case EIntermediateType::Function:
	{
		vector<FExpression> ArgumentExpressions;
		for (auto& Argument : Arguments)
			ArgumentExpressions.emplace_back(Argument->Calculate());
		LeftTerms.emplace_back(Function, ArgumentExpressions);
	}
	break;
	default:
		break;
	}
	return LeftTerms;
}

FExpression SIntermediate::Calculate() const
{
	return FExpression(CalculateTerms());
}