#pragma once
#include <math.h>

class FMath
{
public:
	static const float Pi;

	static float Sin(float Theta)
	{
		return sin(Theta);
	}

	static float Cos(float Theta)
	{
		return cosf(Theta);
	}

	static float Tan(float Theta)
	{
		return tanf(Theta);
	}

	static float Acos(float X)
	{
		return acosf(X);
	}

	static float Max(float X, float Y)
	{
		return fmaxf(X, Y);
	}

	static float Min(float X, float Y)
	{
		return fminf(X, Y);
	}

	static float Pow(float X, float Y)
	{
		return powf(X, Y);
	}

	static float Sqrt(float X)
	{
		return FMath::Sqrt(X);
	}

	static float Log(float X)
	{
		return logf(X);
	}	
};	

