#pragma once

struct SOperation;
using regptr = uint16;

enum class EValueType : uint8
{
	Int, Real, Bool, Unknown
};

struct SType
{
	EValueType ValueType;
	uint8 GrandchildCount;
	uint8 ChildCount;
	uint8 Count;

	SType() : ValueType(EValueType::Unknown) {}
	SType(EValueType ValueType)
		: ValueType(ValueType), GrandchildCount(1), ChildCount(1), Count(1) {}
	SType(EValueType ValueType, uint8 GrandchildCount, uint8 ChildCount, uint8 Count)
		: ValueType(ValueType), GrandchildCount(GrandchildCount), ChildCount(ChildCount), Count(Count) {}

	void Extract(void* Result, const void* Target, uint Index) const;
	void Insert(void* Result, const void* Target, uint Index) const;
	void Calculate(const SOperation& Operation, void* Result, const void* Left, const void* Right) const;
	void Calculate(const SOperation& Operation, void* Result, const void* Target) const;
	void Convert(EValueType DestType, void* Result, const void* Target) const;
	string ToString() const;
	string GetVectorString() const;
	regptr GetSize() const;
	uint8 GetTotalCount() const { return GrandchildCount * ChildCount * Count; }
	uint8 GetUnitCount() const { return GrandchildCount * ChildCount; }
	bool Equals(const SType& Source) const;
	bool CanBloadcast(const SType& Source) const;

	static SType Int;
	static SType Int2;
	static SType Int3;
	static SType Int4;
	static SType Int4x2;
	static SType Real;
	static SType Real2;
	static SType Real3;
	static SType Real3x3;
	static SType Real3x4;
	static SType Real3x4x2;
	static SType Bool;
};