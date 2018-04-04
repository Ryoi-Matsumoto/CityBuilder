#pragma once
#include "UIGrid.h"

class FUIScrollBar;

class FUIScrollable
{
	friend class FUIScrollView;

private:
	uint ViewStartIndex;
	uint AllItemCount;
	uint ViewItemCount;
	uint ViewRangeCunt;
	TUIEvent<> UpdateScrollBar;

public:
	uint GetViewStartIndex() { return ViewStartIndex; }
	uint GetViewItemCount() { return ViewItemCount; }
	void SetAllItemCount(uint Count) { AllItemCount = Count; UpdateScrollBar(); }
	void SetViewStartIndex(uint Index) { ViewStartIndex = Index; UpdateScrollBar(); }
	virtual void OnScrolled() = 0;
};

class FUIScrollView : public FUIControlContainer
{
private:
	FUIScrollBar* ScrollBar;

protected:
	void OnInitialize(FUIApplication* Application) override;
	void OnAreaChanged() override;

public:
	FUIScrollView();

	UIAttribute(FUIScrollable*, ScrollableControl)
	UIAttribute(FUIControl*, ScrolledControl)
};