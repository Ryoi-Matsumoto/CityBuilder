#pragma once
#include "..\Public\MExpression.h"

vector<FExpression> MatchExpressions(
	EExpressionType Type,
	vector<FExpression> const& Source,
	vector<FExpression> const& Targets,
	FMatchingTable& MatchingTable);

bool MatchElementsEachSameIndex(
	vector<FExpression> const& SourceElements,
	vector<FExpression> const& TargetElements,
	FMatchingTable& MatchingTable);

bool MatchElementsWithoutIndexMatching(
	EExpressionType Type,
	vector<FExpression> const& SourceElements,
	vector<FExpression> const& TargetElements,
	FMatchingTable& MatchingTable);

bool MatchHiddenConstant(
	bool IsZeroNotOne,
	FExpression const& Source,
	vector<FExpression> const& TargetElements,
	FMatchingTable& MatchingTable);