#pragma once
#include "..\Container\UIScrollView.h"
#include "Math\Public\MExpression.h"
#include "Rendering\Public\RTextBlock.h"

class FUIFormula : public FUIScrollView
{
private:
	SUIViewRange ViewRange;
	class FUIProcess* ProcessView;
	class FUIRule* RuleView;
	FManipulation Model;
	FRuleTable Rule;
	void OnInitialize() override;

public:
	FUIFormula();

	void SetExpression(FExpression const& Expression);
};