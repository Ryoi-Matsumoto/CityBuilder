#include "../../Public/System/Dynamic.h"
#include "../../Public/System/Debug.h"

struct SDynamicHeapValue
{
	static vector<SDynamicHeap> DefaultArray;
	static map<SDynamicHeap::TString, SDynamicHeap> DefaultObject;
	
	virtual EDynamicType GetType() const { return EDynamicType::Null; }
	virtual SDynamicHeap::TInteger GetIntger() const { assert(false); return 0; }
	virtual SDynamicHeap::TDecimal GetDecimal() const { assert(false); return 0; }
	virtual SDynamicHeap::TString GetString() const { assert(false); return ""; }
	virtual bool GetBoolean() const { assert(false); return true; }
	virtual vector<SDynamicHeap>& GetArray() { assert(false); return DefaultArray; }
	virtual map<SDynamicHeap::TString, SDynamicHeap>& GetObject() { assert(false); return DefaultObject; }
};

vector<SDynamicHeap> SDynamicHeapValue::DefaultArray;
map<SDynamicHeap::TString, SDynamicHeap> SDynamicHeapValue::DefaultObject;

struct SDynamicValueInteger : SDynamicHeapValue
{
	SDynamicHeap::TInteger Value;
	SDynamicHeap::TInteger GetIntger() const override { return Value; }
	EDynamicType GetType() const { return EDynamicType::Integer; }
	SDynamicValueInteger(SDynamicHeap::TInteger Value) : Value(Value) {}
};

struct SDynamicValueDecimal : SDynamicHeapValue
{
	SDynamicHeap::TDecimal Value;
	SDynamicHeap::TDecimal GetDecimal() const override { return Value; }
	EDynamicType GetType() const { return EDynamicType::Decimal; }
	SDynamicValueDecimal(SDynamicHeap::TDecimal Value) : Value(Value) {}
};

struct SDynamicValueString : SDynamicHeapValue
{
	SDynamicHeap::TString Value;
	SDynamicHeap::TString GetString() const override { return Value; }
	EDynamicType GetType() const { return EDynamicType::String; }
	SDynamicValueString(SDynamicHeap::TString Value) : Value(Value) {}
};

struct SDynamicValueBoolean : SDynamicHeapValue
{
	bool Value;
	bool GetBoolean() const override { return Value; }
	EDynamicType GetType() const { return EDynamicType::Boolean; }
	SDynamicValueBoolean(bool Value) : Value(Value) {}
};

struct SDynamicValueArray : SDynamicHeapValue
{
	vector<SDynamicHeap> Value;
	vector<SDynamicHeap>& GetArray() override { return Value; }
	EDynamicType GetType() const { return EDynamicType::Array; }
	SDynamicValueArray(const vector<SDynamicHeap>& Value) : Value(Value) {}
	SDynamicValueArray(vector<SDynamicHeap>&& Value) : Value(move(Value)) {}
};

struct SDynamicValueObject : SDynamicHeapValue
{
	map<SDynamicHeap::TString, SDynamicHeap> Value;
	map<SDynamicHeap::TString, SDynamicHeap>& GetObject() override { return Value; }
	EDynamicType GetType() const { return EDynamicType::Object; }
	SDynamicValueObject(const vector<pair<SDynamicHeap::TString, SDynamicHeap>>& InValue) 
	{
		for (auto Pair : InValue) Value[Pair.first] = Pair.second;
	}
	SDynamicValueObject(vector<pair<SDynamicHeap::TString, SDynamicHeap>>&& InValue)
	{
		for (auto Pair : InValue) Value[Pair.first] = Pair.second;
	}
};

SDynamicHeap::SDynamicHeap() {Ptr = make_shared<SDynamicHeapValue>(); }
SDynamicHeap::SDynamicHeap(TInteger Value) { Ptr = make_shared<SDynamicValueInteger>(Value); }
SDynamicHeap::SDynamicHeap(TDecimal Value) { Ptr = make_shared<SDynamicValueDecimal>(Value); }
SDynamicHeap::SDynamicHeap(TString Value) { Ptr = make_shared<SDynamicValueString>(Value); }
SDynamicHeap::SDynamicHeap(bool Value) { Ptr = make_shared<SDynamicValueBoolean>(Value); }
SDynamicHeap::SDynamicHeap(const vector<SDynamicHeap>& Value) { Ptr = make_shared<SDynamicValueArray>(Value); }
SDynamicHeap::SDynamicHeap(vector<SDynamicHeap>&& Value) { Ptr = make_shared<SDynamicValueArray>(move(Value)); }
SDynamicHeap::SDynamicHeap(const vector<pair<TString, SDynamicHeap>>& Values) { Ptr = make_shared<SDynamicValueObject>(Values); }
SDynamicHeap::SDynamicHeap(vector<pair<TString, SDynamicHeap>>&& Values) { Ptr = make_shared<SDynamicValueObject>(move(Values)); }

SDynamicHeap::TInteger SDynamicHeap::GetInteger() const { return Ptr->GetIntger(); }
SDynamicHeap::TDecimal SDynamicHeap::GetDecimal() const { return Ptr->GetDecimal(); }
SDynamicHeap::TString SDynamicHeap::GetString() const { return Ptr->GetString(); }
bool SDynamicHeap::GetBoolean() const { return Ptr->GetBoolean(); }
vector<SDynamicHeap> SDynamicHeap::GetArray() const { return Ptr->GetArray(); }
EDynamicType SDynamicHeap::GetType() const { return Ptr->GetType(); }
SDynamicHeap SDynamicHeap::operator [](TString Key) { return Ptr->GetObject()[Key]; }
SDynamicHeap SDynamicHeap::operator [](int Index) { return Ptr->GetArray()[Index]; }