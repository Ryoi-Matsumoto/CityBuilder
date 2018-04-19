#pragma once
#include "Container\UIGrid.h"

class FUIHorizontalStackPanel;
class FUIFrame;

class FUIDokingTab : public FUIGrid
{
private:
	FUIHorizontalStackPanel* Header;
	FUIFrame* Frame;

private:
	void OnInitialize(FUIApplication* Application) override;

public:
	FUIDokingTab();
	void AddTabItem(FUIControl* Control, wstring Title);
};