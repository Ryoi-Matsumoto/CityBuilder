#pragma once
#include "..\..\Framework\UIControl.h"

class FUIVerticalStackPanel : public FUIControlContainer
{
private:
	void OnAreaChanged() override;

public:
	FUIVerticalStackPanel();

	uint GetWidth() const override;
	uint GetHeight() const override;
};

class FUIHorizontalStackPanel : public FUIControlContainer
{
private:
	void OnAreaChanged() override;

public:
	FUIHorizontalStackPanel();

	uint GetWidth() const override;
	uint GetHeight() const override;
};