#pragma once
#include "..\UIControl.h"

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

class FUIGrid : public FUIControl
{
private:
	vector<SUILenght> XDefines;
	vector<SUILenght> YDefines;

private:
	void OnAreaChanged() override final;

public:
	void AddXDefine(SUILenght XDefine) { XDefines.push_back(XDefine); }
	void AddYDefine(SUILenght YDefine) { YDefines.push_back(YDefine); }
}; 