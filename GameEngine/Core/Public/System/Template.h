#pragma once
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <numeric>
#include <stack>
#include <array>

using namespace std;

using uint = unsigned int;
using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned long;
using uint64 = unsigned long long;

using int8 = char;
using int16 = short;
using int32 = long;
using int64 = long long;

template<typename T>
using const_ptr = const T*;

struct binarydata : unique_ptr<char[]>
{
private:
	uint array_size;

public:
	binarydata(uint size)
		: unique_ptr<char[]>(new char[size])
		, array_size(size)
	{
	}

	uint size() const
	{
		return array_size;
	}
};