#include "../../Public/System/FileManager.h"
#include <fstream>

vector<char> FFileManager::ReadBinaryFile(string const& FileName)
{
	ifstream FileStream(FileName, ios::in | ios::binary);

	FileStream.seekg(0, ios_base::end);
	int Size = (int)FileStream.tellg();
	FileStream.seekg(0, ios_base::beg);

	auto Result = vector<char>(Size);
	FileStream.read(Result.data(), Size);
	FileStream.close();

	return Result;
}

string FFileManager::ReadTextFile(string const& FileName)
{
	ifstream FileStream(FileName);

	FileStream.seekg(0, ios_base::end);
	int Size = (int)FileStream.tellg();
	FileStream.seekg(0, ios_base::beg);

	string Result;
	Result.resize(Size);
	FileStream.read(&Result[0], Size);
	FileStream.close();

	return Result;
}

void FFileManager::WriteTextFile(string const& FileName, string const& Content)
{
	ofstream FileStream(FileName, std::ios::trunc);
	FileStream << Content;
	FileStream.close();
}