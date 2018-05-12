#pragma once
#include "..\Public\MExpression.h"

class FValue
{
private:
	mint Numerator;
	muint Denominator;

private:
	static int gcd2(int a, int b)
	{
		int c;
		while (a != 0)
		{
			c = a;
			a = b % a;
			b = c;
		}
		return b;
	}

	static int lcm2(int m, int n)
	{
		return  m * n / gcd2(m, n);
	}

public:
	FValue() : Numerator(1), Denominator(1) {}
	FValue(mint Value) : Numerator(Value), Denominator(1) {}

	FValue Reciprocal() const
	{
		FValue Result;
		int Sign = Denominator > 0 ? 1 : -1;
		Result.Numerator = Denominator * Sign;
		Result.Denominator = (muint)abs(Numerator);
		return Result;
	}

	FValue FlipSign() const
	{
		FValue Result(*this);
		Result.Numerator *= -1;
		return Result;
	}

	void Add(FValue const& Value)
	{
		if (IsNaN() || Value.IsNaN())
		{
			Denominator = 0;
			return;
		}
		muint NewDenominator = lcm(Denominator, Value.Denominator);
		mint NewNumerator = Numerator * (NewDenominator / Denominator);
		NewNumerator += Value.Numerator * (NewDenominator / Value.Denominator);
		Denominator = NewDenominator;
		Numerator = NewNumerator;
	}

	void Mul(FValue const& Value)
	{
		if(IsNaN() || Value.IsNaN())
		{
			Denominator = 0;
			return;
		}
		Numerator *= Value.Numerator;
		Denominator *= Value.Denominator;
		muint Gcd = (muint)gcd(abs(Numerator), (mint)Denominator);
		Numerator /= Gcd;
		Denominator /= Gcd;
	}

	void Pow(FValue Value)
	{
		Numerator = pow(Numerator, abs(Value.Numerator).convert_to<uint>());
		Denominator = pow(Denominator, abs(Value.Numerator).convert_to<uint>());
	}

	bool Equals(FValue const& Target) const
	{
		return Target.Numerator == Numerator && Target.Denominator == Denominator;
	}

	bool IsNaN() const
	{
		return Denominator == 0;
	}

	bool IsZero() const
	{
		return Numerator == 0;
	}

	bool IsOne() const
	{
		return Numerator == 1 && Denominator == 1;
	}

	bool IsFraction() const
	{
		return Denominator != 1;
	}

	double GetApproximation() const
	{
		return (double)Numerator / (double)Denominator;
	}

	FExpression ToExpression() const
	{
		if (IsFraction())
			return FExpression(EExpressionType::Div, { FExpression(Numerator), FExpression(Denominator) });
		else
			return FExpression(Numerator);
	}

	string ToString() const
	{
		if (IsFraction())
			return Numerator.str() + "/" + Denominator.str();
		else
			return Numerator.str();
	}
};

struct STerm
{
	FValue ConstantPart;
	FExpression MainPart;

	FExpression ToExpression() const
	{
		if (ConstantPart.IsNaN())
			return FExpression(EExpressionType::NaN);
		else if (MainPart.IsEmpty())
			return ConstantPart.ToExpression();
		else
			return MainPart.Mul(ConstantPart);
	}
};

class FManyTerms
{
private:
	vector<STerm> Terms;

public:
	const vector<STerm>& GetTerms() const
	{
		return Terms;
	}

	void Add(STerm Term)
	{
		bool Added = false;
		// ex: 3x + 4x = 7x
		for (auto& StockTerm : Terms)
		{
			if (Term.MainPart.Equals(StockTerm.MainPart))
			{
				StockTerm.ConstantPart.Add(Term.ConstantPart);
				Added = true;
				break;
			}
		}
		// ex: 3x + 4y
		if (!Added)
			Terms.push_back(Term);
	}

	STerm ToTerm() const
	{
		if (Terms.size() == 1)
		{
			return Terms.front();
		}
		else
		{
			vector<FExpression> Elements;
			Elements.reserve(Terms.size());
			for (auto& Term : Terms)
			{
				if(!Term.ConstantPart.IsZero())
					Elements.push_back(Term.ToExpression());
			}
			STerm Result;
			Result.MainPart = FExpression(EExpressionType::Add, Elements);
			return Result;
		}
	}
};

struct SPart
{
	FExpression Main;
	FExpression Degree;

	FExpression ToExpression() const
	{
		return FExpression(EExpressionType::Pow, { Main, Degree }).Normalize();
	}
};

class FManyParts
{
private:
	FValue ConstantPart;
	vector<SPart> Parts;

public:
	const vector<SPart>& GetParts() const
	{
		return Parts;
	}

	void Mul(SPart Part)
	{
		bool Added = false;
		for (auto& StockPart : Parts)
		{
			if (Part.Main.Equals(StockPart.Main))
			{
				StockPart.Degree = FExpression(EExpressionType::Add, { StockPart.Degree, Part.Degree });
				Added = true;
				break;
			}
		}
		if (!Added)
			Parts.push_back(Part);
	}

	void Mul(STerm Term)
	{
		ConstantPart.Mul(Term.ConstantPart);

		if (!Term.MainPart.IsEmpty())
		{
			auto Part = Term.MainPart.ToPart();

			bool Added = false;
			for (auto& StockPart : Parts)
			{
				if (Part.Main.Equals(StockPart.Main))
				{
					StockPart.Degree = FExpression(EExpressionType::Add, { StockPart.Degree, Part.Degree });
					Added = true;
					break;
				}
			}

			if (!Added)
			{
				Parts.push_back(Part);
			}
		}
	}

	STerm ToTerm() const
	{
		STerm Result;
		Result.ConstantPart = ConstantPart;
		if (Parts.size() > 0)
		{
			vector<FExpression> Elements;
			Elements.reserve(Parts.size());
			for (auto& Part : Parts)
				Elements.push_back(Part.ToExpression());
			Result.MainPart = FExpression(EExpressionType::Mul, Elements);
		}
		return Result;
	}
};