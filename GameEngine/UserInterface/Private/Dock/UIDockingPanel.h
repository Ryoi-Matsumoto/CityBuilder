#pragma once
#include "..\UIControl.h"
#include "..\Container\UIGrid.h"

struct SUIDockingItem
{
	FUIControl* Control;
	wstring Title;
};

class FUIDockingPanel : public FUIControl
{
private:
	vector<SUIDockingItem> Items;
	class FUIDockingArea* MainArea;

private:
	void OnInitialize() override;

public:
	void DragItem(SUIDockingItem* Item);
	void AddItem(FUIControl* Control, wstring Title);
};

class FUIDockingArea : public FUIGrid
{
private:
	FUIDockingPanel* ParentPanel;

public:

	void SetParentPanel(FUIDockingPanel* InParentPanel) { ParentPanel = InParentPanel; }
	void DragItem(SUIDockingItem* Item) { ParentPanel->DragItem(Item); }
	virtual void DropItem(int2 MousePosition, SUIDockingItem* Item, bool HilightNotDrop) = 0;
	virtual void AddItem(SUIDockingItem* Item) = 0;
};