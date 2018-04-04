#pragma once
#include "Template.h"

class FFileManager
{
public:
	static binarydata ReadBinaryFile(wstring const& FileName);
	static wstring ReadTextFile(wstring const& FileName);
	static void WriteTextFile(wstring const& FileName, wstring const& Content);
};