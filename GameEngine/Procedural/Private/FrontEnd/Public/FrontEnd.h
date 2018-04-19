#pragma once
#include "..\..\Intermediate.h"

struct SIntermediate
{
	vector<SInstruction> Instructions;
	vector<uint8> Data;
	vector<SError> Errors;
	vector<regptr> Labels;
};

SIntermediate GenerateIntermediate(string Source);