#pragma once
#include "Common.h"

class FExpression;

string GetTypeName(EValueType Type);
regptr GetTypeSize(EValueType Type);
EOperationType GetOperationType(EValueType Type);
EOperationType GetOperationType(EValueType LeftType, EValueType RightType);
regptr GenerateCodeStoreArguments(const SCompilingData& Data, const vector<FExpression*> Expressions, regptr ArgumentRegister, uint Index);