#pragma once
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <numeric>
#include <stack>
#include <array>
#include <codecvt> 

using namespace std;

using uint = unsigned __int32;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;

template<typename T>
inline vector<unique_ptr<T>> ToUnique(vector<T*> const& Source)
{
	vector<unique_ptr<T>> Result(Source.size());
	for (uint i = 0; i < Source.size(); i++)
		Result[i] = unique_ptr<T>(Source[i]);
	return move(Result);
}

inline wstring ToWString(string String)
{
	wstring_convert<codecvt_utf8<wchar_t>, wchar_t> Converter;
	return Converter.from_bytes(String);
}

inline string ToString(wstring String)
{
	wstring_convert<codecvt_utf8<wchar_t>, wchar_t> Converter;
	return Converter.to_bytes(String);
}