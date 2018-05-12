#pragma once

class FValue
{
private:
	int Numerator;
	uint Denominator;

public:
	FValue() : Numerator(1), Denominator(1) {}
	FValue(int Value) : Numerator(Value), Denominator(1) {}

	FValue Reciprocal() const;
	void Pow(FValue Value);
	void FlipSign();
	void AddValue(FValue const& Value);
	void MulValue(FValue const& Value);
	bool Equals(FValue const& Target) const;
	bool IsZero() const;
	bool IsOne() const;
	bool IsFraction() const;
	string ToString() const;
};