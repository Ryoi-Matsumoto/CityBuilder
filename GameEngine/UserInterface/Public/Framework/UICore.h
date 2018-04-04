#pragma once
#include <functional>

struct HWND__;
using HWND = HWND__*;
struct HINSTANCE__;
using HINSTANCE = HINSTANCE__*;

template<typename... ArgTypes>
using TUIEvent = std::function<void(ArgTypes... Args)>;

#define UIAttribute(Type, Name)					\
Type _##Name;									\
auto&& Name(Type const& Value)					\
{												\
	_##Name = Value;							\
	return move(*this);								\
}								

#define UIAttributeOverride(Type, Name)			\
auto&& Name(Type const& Value) 					\
{												\
	_##Name = Value;							\
	return move(*this);								\
}					

#define UIBase (*this)

#define UINew(ControlType) (*(new ControlType()))

#define UINewNamed(Name, ControlType) (*(Name = new ControlType()))

template<typename T>
class TUIProperty 
{
private:
	function<T()>         Get;
	function<T(const T&)> Set;

public:
	TUIProperty() { }
	TUIProperty(const TUIProperty& other) { }
	TUIProperty& operator=(const TUIProperty& other) { Set(other.Get()); return *this; }

	void operator()(const function<T()>& InGet, const function<T(const T&)>& InSet) 
	{
		if (!Get) Get = InGet;
		if (!Set) Set = InSet;
	}

	T operator()() const { return Get(); }
	T operator()(T const& value) { return Set(value); }

	operator T() const { return Get(); }
	T operator=(T const& value) { return Set(value); }
};

template<typename... ArgTypes>
class TUIStackEvent
{
private:
	using TFunction = function<void(ArgTypes... Args)>;
	vector<TFunction> Events;

public:
	void operator()(ArgTypes... Args)
	{
		for(auto Event : Events)Event(Args);
	}

	void operator+=(TFunction Function)
	{
		Events.push_back(Function);
	}
};

template<typename T>
class TUIInteractiveProperty
{
private:
	T Value;

private:
	T SetValue(T InValue)
	{
		auto PrevValue = Value;
		if (PrevValue != InValue)
		{
			Value = InValue;
			OnChanged(Value);
		}
		return Value;
	}

public:
	TUIStackEvent<void(T)> OnChanged;

public:
	TUIInteractiveProperty() {}
	TUIInteractiveProperty(const TUIInteractiveProperty& Other) { }

	TUIInteractiveProperty& operator=(const TUIInteractiveProperty& Other)
	{
		SetValue(Other.Value); 
		return *this; 
	}

	T operator()() const { return Value; }
	T operator()(T const& InValue) { return SetValue(InValue); }

	operator T() const { return Value; }
	T operator=(T const& InValue) { return SetValue(InValue); }
};