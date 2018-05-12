#pragma once
#include "Template.h"

class FFileManager
{
public:
	static vector<char> ReadBinaryFile(string const& FileName);
	static string ReadTextFile(string const& FileName);
	static void WriteTextFile(string const& FileName, string const& Content);
};