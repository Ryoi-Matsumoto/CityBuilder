#pragma once
#include "..\UIControl.h"

class FUIVerticalStackPanel : public FUIControl
{
private:
	void OnAreaChanged() override final;

public:
	uint GetActualHeight() const override final;
};

class FUIHorizontalStackPanel : public FUIControl
{
private:
	void OnAreaChanged() override final;

public:
	uint GetActualWidth() const override final;
};