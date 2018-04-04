#pragma once
#include "Template.h"
#include <assert.h>

class FDebugger
{
public:
	static void OutputError(wstring const& Message);
};

//#define Error(X) assert(!(X)); FDebugger::OutputError(X);