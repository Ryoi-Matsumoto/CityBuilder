#pragma once
#include "Container\UIGrid.h"
#include "Rendering\Public\RViewport.h"

class FUIWindowControl : public FUIGrid
{
private:
	void OnAreaChanged() override;

public:
	FUIWindowControl();
};