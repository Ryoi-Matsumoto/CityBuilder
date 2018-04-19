#pragma once
#include "UIContainer.h"

class FUIVerticalStackPanel : public FUIContainer
{
private:
	void OnAreaChanged() override;

public:
	uint GetActualWidth() const override;
	uint GetActualHeight() const override;
};

class FUIHorizontalStackPanel : public FUIContainer
{
private:
	void OnAreaChanged() override;

public:
	uint GetActualWidth() const override;
	uint GetActualHeight() const override;
};