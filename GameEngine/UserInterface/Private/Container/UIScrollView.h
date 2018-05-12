#pragma once
#include "..\Container\UIGrid.h"

class FUIScrollView : public FUIGrid, public FUIFrame<FUIControl>
{
private:
	class FUIScrollBar* ScrollBar;
	SUIViewRange* Range;

protected:
	void OnInitialize() override;
	void OnMouseWheel(int Delta, int2 MousePosition) override final;

public:
	FUIScrollView() {}

	void SetViewRange(SUIViewRange* InRange) { Range = InRange; }
};

class FUIScrollable
{
protected:
	SUIViewRange ViewRange;

public:
	SUIViewRange* GetViewRange() { return &ViewRange; }
};