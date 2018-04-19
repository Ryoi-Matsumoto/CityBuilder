#include "Type.h"
#include "Intermediate.h"

template<typename TResult, typename TTarget>
void ConvertVector(uint Size, TResult* Result, const TTarget* Target)
{
	for (uint i = 0; i < Size; i++)
		Result[i] = (TResult)Target[i];
}

template<typename T>
void CalculateSingle(EInstruction Code, T* Result, T Left, T Right)
{
	switch (Code)
	{
	case EInstruction::ADD:
		*Result = Left + Right;
		break;
	case EInstruction::SUB:
		*Result = Left - Right;
		break;
	case EInstruction::MUL:
		*Result = Left * Right;
		break;
	case EInstruction::DIV:
		*Result = Left / Right;
		break;
	case EInstruction::MOD:
		*Result = (int)Left % (int)Right;
		break;
	case EInstruction::AND:
		*Result = Left && Right;
		break;
	case EInstruction::OR:
		*Result = Left || Right;
		break;
	case EInstruction::CMPE:
		*Result = Left == Right;
		break;
	case EInstruction::CMPNE:
		*Result = Left != Right;
		break;
	case EInstruction::CMPG:
		*Result = Left < Right;
		break;
	case EInstruction::CMPL:
		*Result = Left > Right;
		break;
	case EInstruction::CMPGE:
		*Result = Left <= Right;
		break;
	case EInstruction::CMPLE:
		*Result = Left >= Right;
		break;
	default:
		assert(false);
	}
}

template<typename T>
void CalculateFlipSign(uint Size, T* Result, const T* Target)
{
	for (uint i = 0; i < Size; i++)
		Result[i] = -Target[i];
}

template<typename T>
void CalculateVector(EInstruction Code, uint Size, T* Result, const T* Left, const T* Right)
{
	switch (Code)
	{
	case EInstruction::ADD:
		for (uint i = 0; i < Size; i++)
			Result[i] = Left[i] + Right[i];
		break;
	case EInstruction::SUB:
		for (uint i = 0; i < Size; i++)
			Result[i] = Left[i] - Right[i];
		break;
	case EInstruction::MUL:
		for (uint i = 0; i < Size; i++)
			Result[i] = Left[i] * Right[i];
		break;
	case EInstruction::DIV:
		for (uint i = 0; i < Size; i++)
			Result[i] = Left[i] / Right[i];
		break;
	default:
		assert(false);
	}
}

template<typename T>
void CalculateVectorWithBloadcast(EInstruction Code, uint Count, uint Unit, T* Result, const T* Left, const T* Right)
{
	switch (Code)
	{
	case EInstruction::ADD:
		for (uint i = 0; i < Count; i++)
			for (uint j = 0; j < Unit; j++)
				Result[j + i * Unit] = Left[j + i * Unit] + Right[j];
		break;
	case EInstruction::SUB:
		for (uint i = 0; i < Count; i++)
			for (uint j = 0; j < Unit; j++)
				Result[j + i * Unit] = Left[j + i * Unit] - Right[j];
		break;
	case EInstruction::MUL:
		for (uint i = 0; i < Count; i++)
			for (uint j = 0; j < Unit; j++)
				Result[j + i * Unit] = Left[j + i * Unit] * Right[j];
		break;
	case EInstruction::DIV:
		for (uint i = 0; i < Count; i++)
			for (uint j = 0; j < Unit; j++)
				Result[j + i * Unit] = Left[j + i * Unit] / Right[j];
		break;
	default:
		assert(false);
	}
}

void SType::Calculate(const SOperation& Operation, void* Result, const void* Left, const void* Right) const
{
	uint Size = GetTotalCount();
	uint Unit = GetUnitCount();

	if (Operation.IsBloadcast)
	{
		switch (ValueType)
		{
		case EValueType::Int:
			CalculateVectorWithBloadcast(Operation.Code, Count, Unit, (int*)Result, (int*)Left, (int*)Right);
			break;
		case EValueType::Real:
			CalculateVectorWithBloadcast(Operation.Code, Count, Unit, (float*)Result, (float*)Left, (float*)Right);
			break;
		default:
			assert(false);
		}
	}
	else
	{
		switch (ValueType)
		{
		case EValueType::Int:
			if(GrandchildCount == ChildCount == Count == 1)
				CalculateSingle(Operation.Code, (int*)Result, *(int*)Left, *(int*)Right);
			else
				CalculateVector(Operation.Code, Size, (int*)Result, (int*)Left, (int*)Right);
			break;
		case EValueType::Real:
			if (GrandchildCount == ChildCount == Count == 1)
				CalculateSingle(Operation.Code, (float*)Result, *(float*)Left, *(float*)Right);
			else
				CalculateVector(Operation.Code, Size, (float*)Result, (float*)Left, (float*)Right);
			break;
		default:
			assert(false);
		}
	}
}

void SType::Calculate(const SOperation& Operation, void* Result, const void* Target) const
{
	uint Size = GetTotalCount();

	switch (Operation.Code)
	{
	case EInstruction::MNS:
		switch (ValueType)
		{
		case EValueType::Int:
			if (Count == 1)
				*(int*)Result = -(*(int*)Target);
			else
				CalculateFlipSign(Size, (int*)Result, (int*)Target);
			break;
		case EValueType::Real:
			if (Count == 1)
				*(float*)Result = -(*(float*)Target);
			else
				CalculateFlipSign(Size, (float*)Result, (float*)Target);
			break;
		default:
			assert(false);
		}
	case EInstruction::NOT:
		switch (ValueType)
		{
		case EValueType::Bool:
			*(bool*)Result = !(*(bool*)Target);
			break;
		default:
			assert(false);
		}
	case EInstruction::SQRT:
		switch (ValueType)
		{
		case EValueType::Real:
			*(float*)Result = sqrt(*(float*)Target);
			break;
		default:
			assert(false);
		}
		break;
	default:
		assert(false);
	}
}

void SType::Convert(EValueType DestType, void* Result, const void* Target) const
{
	uint Size = GetTotalCount();

	switch (DestType)
	{
	case EValueType::Int:
		switch (ValueType)
		{
		case EValueType::Real:
			ConvertVector(Size, (int*)Result, (float*)Target);
			break;
		default:
			assert(false);
		}
		break;
	case EValueType::Real:		
		switch (ValueType)
		{
		case EValueType::Int:
			ConvertVector(Size, (float*)Result, (int*)Target);
			break;
		default:
			assert(false);
		}
		break;
	default:
		assert(false);
	}
}

void SType::Extract(void* Result, const void* Target, uint Index) const
{
	uint Unit = GetUnitCount();

	switch (ValueType)
	{
	case EValueType::Int:
	case EValueType::Real:
		memcpy(Result, (int*)Target + Unit * Index, Unit);
	default:
		assert(false);
	}
}

void SType::Insert(void* Result, const void* Target, uint Index) const
{
	uint Unit = GetUnitCount();

	switch (ValueType)
	{
	case EValueType::Int:
	case EValueType::Real:
		memcpy((int*)Result + Unit * Index, Target, Unit);
	default:
		assert(false);
	}
}

string SType::GetVectorString() const
{
	if (GrandchildCount > 1)
		return to_string(GrandchildCount) + "x" + to_string(ChildCount) + "x" + to_string(Count);
	else if (ChildCount > 1)
		return to_string(ChildCount) + "x" + to_string(Count);
	else if (Count > 1)
		return to_string(Count);
	else
		return "";
}

string SType::ToString() const
{
	switch (ValueType)
	{
	case EValueType::Int:
		return "int" + GetVectorString();
	case EValueType::Real:
		return "real" + GetVectorString();
	case EValueType::Bool:
		return "bool";
	default:
		return "unknown";
	}
}

regptr SType::GetSize() const
{
	switch (ValueType)
	{
	case EValueType::Real: 
		return sizeof(float) * GetTotalCount();
	case EValueType::Int: 
		return sizeof(long) * GetTotalCount();
	case EValueType::Bool: 
		return sizeof(bool);
	}
	return 0;
}

bool SType::Equals(const SType& Source) const
{
	return ValueType == Source.ValueType
		&& GrandchildCount == Source.GrandchildCount
		&& ChildCount == Source.ChildCount
		&& Count == Source.Count;
}

bool SType::CanBloadcast(const SType& Source) const
{
	return ValueType == Source.ValueType
		&& Source.GrandchildCount == 1
		&& GrandchildCount == Source.ChildCount
		&& ChildCount == Source.Count;
}

SType SType::Int = SType(EValueType::Int);
SType SType::Int2 = SType(EValueType::Int, 1, 1, 2);
SType SType::Int3 = SType(EValueType::Int, 1, 1, 3);
SType SType::Int4 = SType(EValueType::Int, 1, 1, 4);
SType SType::Int4x2 = SType(EValueType::Int, 1, 4, 2);
SType SType::Real = SType(EValueType::Real);
SType SType::Real2 = SType(EValueType::Real, 1, 1, 2);
SType SType::Real3 = SType(EValueType::Real, 1, 1, 3);
SType SType::Real3x3 = SType(EValueType::Real, 1, 3, 3);
SType SType::Real3x4 = SType(EValueType::Real, 1, 3, 4);
SType SType::Real3x4x2 = SType(EValueType::Real, 3, 4, 2);
SType SType::Bool = SType(EValueType::Bool);