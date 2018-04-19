#pragma once
#include "..\Container\UIGrid.h"

class FUIScrollView : public FUIGrid, public FUIFrame<FUIControl>
{
private:
	class FUIScrollBar* ScrollBar;
	SUIViewRange* Range;

protected:
	void OnInitialize(FUIApplication* Application) override;
	void OnMouseWheel(int Delta, int2 MousePosition) override;

public:
	FUIScrollView() {}

	void SetViewRange(SUIViewRange* InRange) { Range = InRange; }
};