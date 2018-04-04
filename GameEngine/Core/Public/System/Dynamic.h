#pragma once
#include "Template.h"

enum class EDynamicType
{
	Null, Integer, Decimal, String, Boolean, Object, Array
};

struct SDynamicStack
{
	using TInteger = int;
	using TDecimal = float;
	using TString = string;
	using TPair = pair<TString, SDynamicStack>;

	struct SVectorWrapper
	{
		vector<SDynamicStack> Body;
	};

	struct SMapWrapper
	{
		map<TString, SDynamicStack> Body;
	};

	struct SStringWrapper
	{
		TString Body;
	};

	EDynamicType Type;

	SDynamicStack() : Type(EDynamicType::Null) {}
	SDynamicStack(TInteger Value) : Type(EDynamicType::Integer), Integer(Value) {}
	SDynamicStack(TDecimal Value) : Type(EDynamicType::Decimal), Decimal(Value) {}
	SDynamicStack(const TString& Value) : Type(EDynamicType::String)
	{
		String.Body = Value;
	}
	SDynamicStack(TString&& Value) : Type(EDynamicType::String)
	{
		String.Body = move(Value);
	}
	SDynamicStack(bool Value) : Type(EDynamicType::Boolean), Boolean(Value) {}
	SDynamicStack(const vector<SDynamicStack>& Value) : Type(EDynamicType::Array) 
	{
		Array.Body = Value;
	}
	SDynamicStack(vector<SDynamicStack>&& Value) : Type(EDynamicType::Array)
	{
		Array.Body = move(Value);
	}
	SDynamicStack(const vector<TPair>& Value) : Type(EDynamicType::Object)
	{
		for (auto Pair : Value) Object.Body[Pair.first] = Pair.second;
	}

	TInteger GetInteger() const { return Integer; }
	TDecimal GetDecimal() const { return Decimal; }
	TString GetString() const { return String.Body; }
	bool GetBoolean() const { return Boolean; }
	vector<SDynamicStack> GetArray() const { return vector<SDynamicStack>(); }
	EDynamicType GetType() const { return Type; }
	SDynamicStack operator [](TString Key) { return Object.Body[Key]; }
	SDynamicStack operator [](int Index) { return Array.Body[Index]; }

	struct
	{
		SDynamicStack::TDecimal Decimal;
		SDynamicStack::TInteger Integer;
		SStringWrapper String;
		bool Boolean;
		SVectorWrapper Array;
		SMapWrapper Object;
	};
};

struct SDynamicHeapValue;

struct SDynamicHeap
{
	using TInteger = int;
	using TDecimal = float;
	using TString = string;
	using TPair = pair<TString, SDynamicHeap>;

	SDynamicHeap();
	SDynamicHeap(TInteger Value);
	SDynamicHeap(TDecimal Value);
	SDynamicHeap(TString Value);
	SDynamicHeap(bool Value);
	SDynamicHeap(const vector<SDynamicHeap>& Value);
	SDynamicHeap(vector<SDynamicHeap>&& Value);
	SDynamicHeap(const vector<TPair>& Value);
	SDynamicHeap(vector<TPair>&& Value);

	TInteger GetInteger() const;
	TDecimal GetDecimal() const;
	TString GetString() const;
	bool GetBoolean() const;
	vector<SDynamicHeap> GetArray() const;
	EDynamicType GetType() const;
	SDynamicHeap operator [](TString Key);
	SDynamicHeap operator [](int Index);

private:
	shared_ptr<SDynamicHeapValue> Ptr;
};

using SDynamic = SDynamicHeap;