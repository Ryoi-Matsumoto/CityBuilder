#include "Utility.h"
#include "Expression.h"

string GetTypeName(EValueType Type)
{
	switch (Type)
	{
	case EValueType::Error:return "unknown";
	case EValueType::Int:return "int";
	case EValueType::Int2:return "int2";
	case EValueType::Int3:return "int3";
	case EValueType::Int3x4:return "int3x4";
	case EValueType::Int3x8:return "int3x8";
	case EValueType::Real:return "real";
	case EValueType::Real2:return "real2";
	case EValueType::Real3:return "real3";
	case EValueType::Real3x4:return "real3x4";
	case EValueType::Real3x8:return "real3x8";
	case EValueType::Boolean:return "bool";
	}
	assert(false);
	return "unknown";
}

regptr GetTypeSize(EValueType Type)
{
	switch (Type)
	{
	case EValueType::Real: return sizeof(float);
	case EValueType::Int: return sizeof(long);
	case EValueType::Boolean: return sizeof(bool);
	}
	return 0;
}

EOperationType GetOperationType(EValueType Type)
{
	switch (Type)
	{
	case EValueType::Int:return EOperationType::II;
	case EValueType::Int2:return EOperationType::I2I2;
	case EValueType::Int3:return EOperationType::I3I3;
	case EValueType::Real:return EOperationType::RR;
	case EValueType::Real2:return EOperationType::R2R2;
	case EValueType::Real3:return EOperationType::R3R3;
	case EValueType::Boolean:return EOperationType::BB;
	}
	return EOperationType::Error;
}

EOperationType GetOperationType(EValueType LeftType, EValueType RightType)
{
	switch (LeftType)
	{
	case EValueType::Int:
		switch (RightType)
		{
		case EValueType::Int:return EOperationType::II;
		}
		break;
	case EValueType::Int2:
		switch (RightType)
		{
		case EValueType::Int:return EOperationType::I2I;
		case EValueType::Int2:return EOperationType::I2I2;
		}
		break;
	case EValueType::Int3:
		switch (RightType)
		{
		case EValueType::Int:return EOperationType::I3I;
		case EValueType::Int3:return EOperationType::I3I3;
		}
		break;
	case EValueType::Int3x4:
		switch (RightType)
		{
		case EValueType::Int:return EOperationType::I3x4I;
		case EValueType::Int3:return EOperationType::I3x4I3;
		case EValueType::Int3x4:return EOperationType::I3x4I3x4;
		}
		break;
	case EValueType::Int3x8:
		switch (RightType)
		{
		case EValueType::Int:return EOperationType::I3x8I;
		case EValueType::Int3:return EOperationType::I3x8I3;
		case EValueType::Int3x4:return EOperationType::I3x8I3x4;
		case EValueType::Int3x8:return EOperationType::I3x8I3x8;
		}
		break;
	case EValueType::Real:
		switch (RightType)
		{
		case EValueType::Real:return EOperationType::RR;
		}
		break;
	case EValueType::Real2:
		switch (RightType)
		{
		case EValueType::Real:return EOperationType::R2R;
		case EValueType::Real2:return EOperationType::R2R2;
		}
		break;
	case EValueType::Real3:
		switch (RightType)
		{
		case EValueType::Real:return EOperationType::R3R;
		case EValueType::Real3:return EOperationType::R3R3;
		}
		break;
	case EValueType::Real3x4:
		switch (RightType)
		{
		case EValueType::Real:return EOperationType::R3x4R;
		case EValueType::Real3:return EOperationType::R3x4R3;
		case EValueType::Real3x4:return EOperationType::R3x4R3x4;
		}
		break;
	case EValueType::Real3x8:
		switch (RightType)
		{
		case EValueType::Real:return EOperationType::R3x8R;
		case EValueType::Real3:return EOperationType::R3x8R3;
		case EValueType::Real3x4:return EOperationType::R3x8R3x4;
		case EValueType::Real3x8:return EOperationType::R3x8R3x8;
		}
		break;
	case EValueType::Boolean:
		switch (RightType)
		{
		case EValueType::Boolean:return EOperationType::BB;
		}
		break;
	}
	return EOperationType::Error;
}

regptr GenerateCodeStoreArguments(const SCompilingData& Data, const vector<FExpression*> Expressions, regptr ArgumentRegister, uint Index)
{
	for (auto Expression : Expressions)
	{
		auto ResultRegister = Expression->GenerateCode(Data, ArgumentRegister);
		if (ResultRegister != ArgumentRegister)ArgumentRegister += GetTypeSize(Expression->GetType());
		auto OperationCode = GetOperationType(Expression->GetType());
		Data.Routine->AddInstruction(EInstruction::STOREA, OperationCode, ArgumentRegister, Index, 0);
		Index++;
	}
	return ArgumentRegister;
}