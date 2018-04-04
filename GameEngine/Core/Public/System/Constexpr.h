#pragma once
#include "Template.h"

constexpr uint StringLength(const char* String, uint Index = 0)
{
	return String[Index] == '\0' ? Index : StringLength(String, Index + 1);
}

constexpr uint64 ToUint64(const char* String, uint Offset, uint64 U64 = 0)
{
	return Offset ? ToUint64(String, Offset - 1, (U64 << CHAR_BIT) | String[Offset - 1]) : U64;
}

constexpr uint64 ToUint64(const char* String)
{
	return ToUint64(String, StringLength(String));
}

constexpr uint64 operator"" u64(const char* String, size_t Length)
{
	return ToUint64(String, Length);
}