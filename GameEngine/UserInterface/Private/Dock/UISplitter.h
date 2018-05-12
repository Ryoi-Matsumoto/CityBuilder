#pragma once
#include "UIDockingPanel.h"

class FUIHorizontalSplitter : public FUIDockingArea
{
private:
	FUIDockingArea* LeftArea;
	FUIDockingArea* RightArea;

private:
	void OnInitialize() override;

public:
	FUIHorizontalSplitter(FUIDockingArea* LeftArea, FUIDockingArea* RightArea)
		: LeftArea(LeftArea), RightArea(RightArea) {}
};

class FUIVerticalSplitter : public FUIDockingArea
{
private:
	FUIDockingArea* UpperArea;
	FUIDockingArea* LowerArea;

private:
	void OnInitialize() override;

public:
};