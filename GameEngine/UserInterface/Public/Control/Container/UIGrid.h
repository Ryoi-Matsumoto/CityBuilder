#pragma once
#include "..\..\Framework\UIControl.h"

struct SUILenght
{		
	float Lenght;
	bool IsRatio;
	bool IsAuto;

	SUILenght()
		: Lenght(0)
		, IsRatio(false)
		, IsAuto(true)
	{
	}

	SUILenght(float Lenght, bool IsRatio = false)
		: Lenght(Lenght)
		, IsRatio(IsRatio)
		, IsAuto(false)
	{
	}
};

class FUIGrid : public FUIControlContainer
{
private:
	void OnAreaChanged() override;

public:
	uint GetWidth() const override { return 100; }
	uint GetHeight() const override { return 100; }

	UIAttribute(vector<SUILenght>, XDefines)
	UIAttribute(vector<SUILenght>, YDefines)
}; 