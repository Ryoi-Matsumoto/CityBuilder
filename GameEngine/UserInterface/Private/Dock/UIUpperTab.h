#pragma once
#include "UIDockingPanel.h"

class FUIUpperTab : public FUIDockingArea
{
private:
	class FUITabItemHeader* PreviusSelectedHeader;
	class FUITabItemHeader* SelectedHeader;
	class FUIHorizontalStackPanel* HeaderStackPanel;
	FUIControl* Frame;

private:
	void OnInitialize() override;

public:
	FUIUpperTab();

	void DropItem(int2 MousePosition, SUIDockingItem* Item, bool HilightNotDrop) override;
	void AddItem(SUIDockingItem* Item) override;
	void SelectItem(class FUITabItemHeader* Header, FUIControl* Control);
	void RemoveItem(class FUITabItemHeader* Header);
};