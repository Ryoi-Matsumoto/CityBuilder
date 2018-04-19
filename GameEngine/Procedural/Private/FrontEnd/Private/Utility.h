#pragma once
#include "Common.h"

class FExpression;

vector<regptr> GenerateCodeArguments(const SCompilingData& Data, const vector<FExpression*> Arguments);
regptr GenerateCodeStoreArguments(const SCompilingData& Data, const vector<FExpression*> Arguments, regptr ArgumentRegister);
bool CheckArguments(string Type, string Name, const SCompilingData& Data, const vector<SType>& ArgumentTypes, const vector<FExpression*>& Arguments);