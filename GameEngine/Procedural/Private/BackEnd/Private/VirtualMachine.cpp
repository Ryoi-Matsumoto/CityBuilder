#include "VirtualMachine.h"
#include <intrin.h>

/*
予めサイズと型を決定する
storea, storer 後から用途を決定する
loada, loadr, load 前からサイズ, 型,　用途を決定する

loadaとstorerが競合したとき、途中で代入時にfreeするからそのときは両方レジスタが使える

storea レジスタ, インデックス, 種類
*/

#define CALC(T, C) *(T*)(Address + Ins.Operand0) = *(T*)(Address + Ins.Operand1) C *(T*)(Address + Ins.Operand2)

bool FVirtualMachine::Instruct(const SInstruction& Ins)
{
	float R0, R1, R2, R3;
	SShapeIndex ShapeIndex;
	long Indexes[3];

	constexpr int Sf0 = 0;
	constexpr int Sf1 = sizeof(float);
	constexpr int Sf2 = sizeof(float) * 2;

	char* Address = Memory + StackPointer;

	switch (Ins.Code)
	{
	case EInstruction::ADD:
		switch (Ins.Type)
		{
		case EOperationType::II: CALC(int, +); break;
		case EOperationType::I2I2: CALC(int2, +); break;
		case EOperationType::I2I:
		case EOperationType::I3I:
		case EOperationType::I3I3:
		case EOperationType::I3x4I3x4:
		case EOperationType::I3x4I:
		case EOperationType::I3x8I3x8:
		case EOperationType::I3x8I:
		case EOperationType::I3x4I3:
		case EOperationType::I3x8I3:
		case EOperationType::I3x8I3x4:
		case EOperationType::RR:
		case EOperationType::R2R2:
		case EOperationType::R2R:
		case EOperationType::R3R:
		case EOperationType::R3R3:
		case EOperationType::R3x4R3x4:
		case EOperationType::R3x4R:
		case EOperationType::R3x8R3x8:
		case EOperationType::R3x8R:
		case EOperationType::R3x4R3:
		case EOperationType::R3x8R3:
		case EOperationType::R3x8R3x4:
		case EOperationType::BB:
		case EOperationType::None:
		case EOperationType::Error:
		default:
			break;
		}

		*(long*)(Address + Ins.Operand0) = *(long*)(Address + Ins.Operand1) + *(long*)(Address + Ins.Operand2);
		break;
	case EInstruction::SUB:
		*(long*)(Address + Ins.Operand0) = *(long*)(Address + Ins.Operand1) - *(long*)(Address + Ins.Operand2);
		break;
	case EInstruction::MUL:
		*(long*)(Address + Ins.Operand0) = *(long*)(Address + Ins.Operand1) * *(long*)(Address + Ins.Operand2);
		break;
	case EInstruction::DIV:
		*(long*)(Address + Ins.Operand0) = *(long*)(Address + Ins.Operand1) / *(long*)(Address + Ins.Operand2);
		break;
	case EInstruction::MNS:
		*(long*)(Address + Ins.Operand0) = -*(long*)(Address + Ins.Operand1);
		break;
	case EInstruction::MOD:
		*(long*)(Address + Ins.Operand0) = *(long*)(Address + Ins.Operand1) % *(long*)(Address + Ins.Operand2);
		break;
	case EInstruction::FADD:
		*(float*)(Address + Ins.Operand0) = *(float*)(Address + Ins.Operand1) + *(float*)(Address + Ins.Operand2);
		break;
	case EInstruction::FSUB:
		*(float*)(Address + Ins.Operand0) = *(float*)(Address + Ins.Operand1) - *(float*)(Address + Ins.Operand2);
		break;
	case EInstruction::FMUL:
		*(float*)(Address + Ins.Operand0) = *(float*)(Address + Ins.Operand1) * *(float*)(Address + Ins.Operand2);
		break;
	case EInstruction::FDIV:
		*(float*)(Address + Ins.Operand0) = *(float*)(Address + Ins.Operand1) / *(float*)(Address + Ins.Operand2);
		break;
	case EInstruction::FMNS:
		*(float*)(Address + Ins.Operand0) = -*(float*)(Address + Ins.Operand1);
		break;
	case EInstruction::VADD:
		*(float*)(Address + Ins.Operand0 + Sf0) = *(float*)(Address + Ins.Operand1 + Sf0) + *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf1) = *(float*)(Address + Ins.Operand1 + Sf1) + *(float*)(Address + Ins.Operand2 + Sf1);
		*(float*)(Address + Ins.Operand0 + Sf2) = *(float*)(Address + Ins.Operand1 + Sf2) + *(float*)(Address + Ins.Operand2 + Sf2);
		break;
	case EInstruction::VSUB:
		*(float*)(Address + Ins.Operand0 + Sf0) = *(float*)(Address + Ins.Operand1 + Sf0) - *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf1) = *(float*)(Address + Ins.Operand1 + Sf1) - *(float*)(Address + Ins.Operand2 + Sf1);
		*(float*)(Address + Ins.Operand0 + Sf2) = *(float*)(Address + Ins.Operand1 + Sf2) - *(float*)(Address + Ins.Operand2 + Sf2);
		break;
	case EInstruction::VMUL:
		*(float*)(Address + Ins.Operand0 + Sf0) = *(float*)(Address + Ins.Operand1 + Sf0) * *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf1) = *(float*)(Address + Ins.Operand1 + Sf1) * *(float*)(Address + Ins.Operand2 + Sf1);
		*(float*)(Address + Ins.Operand0 + Sf2) = *(float*)(Address + Ins.Operand1 + Sf2) * *(float*)(Address + Ins.Operand2 + Sf2);
		break;
	case EInstruction::VDIV:
		*(float*)(Address + Ins.Operand0 + Sf0) = *(float*)(Address + Ins.Operand1 + Sf0) / *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf1) = *(float*)(Address + Ins.Operand1 + Sf1) / *(float*)(Address + Ins.Operand2 + Sf1);
		*(float*)(Address + Ins.Operand0 + Sf2) = *(float*)(Address + Ins.Operand1 + Sf2) / *(float*)(Address + Ins.Operand2 + Sf2);
		break;
	case EInstruction::VMNS:
		*(float*)(Address + Ins.Operand0 + Sf0) = -*(float*)(Address + Ins.Operand1 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf1) = -*(float*)(Address + Ins.Operand1 + Sf1);
		*(float*)(Address + Ins.Operand0 + Sf2) = -*(float*)(Address + Ins.Operand1 + Sf2);
		break;
	case EInstruction::VFADD:
		*(float*)(Address + Ins.Operand0 + Sf0) = *(float*)(Address + Ins.Operand1 + Sf0) + *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf1) = *(float*)(Address + Ins.Operand1 + Sf1) + *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf2) = *(float*)(Address + Ins.Operand1 + Sf2) + *(float*)(Address + Ins.Operand2 + Sf0);
		break;
	case EInstruction::VFSUB:
		*(float*)(Address + Ins.Operand0 + Sf0) = *(float*)(Address + Ins.Operand1 + Sf0) - *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf1) = *(float*)(Address + Ins.Operand1 + Sf1) - *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf2) = *(float*)(Address + Ins.Operand1 + Sf2) - *(float*)(Address + Ins.Operand2 + Sf0);
		break;
	case EInstruction::VFMUL:
		*(float*)(Address + Ins.Operand0 + Sf0) = *(float*)(Address + Ins.Operand1 + Sf0) * *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf1) = *(float*)(Address + Ins.Operand1 + Sf1) * *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf2) = *(float*)(Address + Ins.Operand1 + Sf2) * *(float*)(Address + Ins.Operand2 + Sf0);
		break;
	case EInstruction::VFDIV:
		*(float*)(Address + Ins.Operand0 + Sf0) = *(float*)(Address + Ins.Operand1 + Sf0) / *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf1) = *(float*)(Address + Ins.Operand1 + Sf1) / *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf2) = *(float*)(Address + Ins.Operand1 + Sf2) / *(float*)(Address + Ins.Operand2 + Sf0);
		break;
	case EInstruction::LN:
		*((bool*)Address + Ins.Operand0) = !*((bool*)Address + Ins.Operand1);
		break;
	case EInstruction::LAND:
		*((bool*)Address + Ins.Operand0) = *((bool*)Address + Ins.Operand1) && *((bool*)Address + Ins.Operand2);
		break;
	case EInstruction::LOR:
		*((bool*)Address + Ins.Operand0) = *((bool*)Address + Ins.Operand1) || *((bool*)Address + Ins.Operand2);
		break;

	case EInstruction::ICE:
		*(long*)(Address + Ins.Operand0) = *(long*)(Address + Ins.Operand1) == *(long*)(Address + Ins.Operand2);
		break;
	case EInstruction::ICNE:
		*(long*)(Address + Ins.Operand0) = *(long*)(Address + Ins.Operand1) != *(long*)(Address + Ins.Operand2);
		break;
	case EInstruction::ICG:
		*(long*)(Address + Ins.Operand0) = *(long*)(Address + Ins.Operand1) < *(long*)(Address + Ins.Operand2);
		break;
	case EInstruction::ICGE:
		*(long*)(Address + Ins.Operand0) = *(long*)(Address + Ins.Operand1) <= *(long*)(Address + Ins.Operand2);
		break;
	case EInstruction::ICL:
		*(long*)(Address + Ins.Operand0) = *(long*)(Address + Ins.Operand1) > *(long*)(Address + Ins.Operand2);
		break;
	case EInstruction::ICLE:
		*(long*)(Address + Ins.Operand0) = *(long*)(Address + Ins.Operand1) >= *(long*)(Address + Ins.Operand2);
		break;

	case EInstruction::FCE:
		*(float*)(Address + Ins.Operand0) = *(float*)(Address + Ins.Operand1) == *(float*)(Address + Ins.Operand2);
		break;
	case EInstruction::FCNE:
		*(float*)(Address + Ins.Operand0) = *(float*)(Address + Ins.Operand1) != *(float*)(Address + Ins.Operand2);
		break;
	case EInstruction::FCG:
		*(float*)(Address + Ins.Operand0) = *(float*)(Address + Ins.Operand1) < *(float*)(Address + Ins.Operand2);
		break;
	case EInstruction::FCGE:
		*(float*)(Address + Ins.Operand0) = *(float*)(Address + Ins.Operand1) <= *(float*)(Address + Ins.Operand2);
		break;
	case EInstruction::FCL:
		*(float*)(Address + Ins.Operand0) = *(float*)(Address + Ins.Operand1) > *(float*)(Address + Ins.Operand2);
		break;
	case EInstruction::FCLE:
		*(float*)(Address + Ins.Operand0) = *(float*)(Address + Ins.Operand1) >= *(float*)(Address + Ins.Operand2);
		break;

	case EInstruction::SSIN:
		*(float*)(Address + Ins.Operand0) = sinf(*(float*)(Address + Ins.Operand1));
		break;
	case EInstruction::SCOS:
		*(float*)(Address + Ins.Operand0) = cosf(*(float*)(Address + Ins.Operand1));
		break;
	case EInstruction::STAN:
		*(float*)(Address + Ins.Operand0) = tanf(*(float*)(Address + Ins.Operand1));
		break;
	case EInstruction::VNORM:
		R0 = *(float*)(Address + Ins.Operand1 + Sf0);
		R1 = *(float*)(Address + Ins.Operand1 + Sf1);
		R2 = *(float*)(Address + Ins.Operand1 + Sf2);
		R3 = FMath::Sqrt(R0 * R0 + R1 * R1 + R2 * R2);
		*(float*)(Address + Ins.Operand0 + Sf0) = R0 / R3;
		*(float*)(Address + Ins.Operand0 + Sf1) = R1 / R3;
		*(float*)(Address + Ins.Operand0 + Sf2) = R2 / R3;
		break;
	case EInstruction::VCROS:
		R0 = *(float*)(Address + Ins.Operand1 + Sf1) * *(float*)(Address + Ins.Operand2 + Sf2) - *(float*)(Address + Ins.Operand1 + Sf2) * *(float*)(Address + Ins.Operand2 + Sf1);
		R1 = *(float*)(Address + Ins.Operand1 + Sf2) / *(float*)(Address + Ins.Operand2 + Sf0) - *(float*)(Address + Ins.Operand1 + Sf0) / *(float*)(Address + Ins.Operand2 + Sf2);
		R2 = *(float*)(Address + Ins.Operand1 + Sf0) / *(float*)(Address + Ins.Operand2 + Sf1) - *(float*)(Address + Ins.Operand1 + Sf1) / *(float*)(Address + Ins.Operand2 + Sf0);
		*(float*)(Address + Ins.Operand0 + Sf0) = R0;
		*(float*)(Address + Ins.Operand0 + Sf1) = R1;
		*(float*)(Address + Ins.Operand0 + Sf2) = R2;
		break;
	case EInstruction::VDOT:
		R0 = *(float*)(Address + Ins.Operand1 + Sf0) * *(float*)(Address + Ins.Operand2 + Sf0);
		R1 = *(float*)(Address + Ins.Operand1 + Sf1) * *(float*)(Address + Ins.Operand2 + Sf2);
		R2 = *(float*)(Address + Ins.Operand1 + Sf2) * *(float*)(Address + Ins.Operand2 + Sf2);
		*(float*)(Address + Ins.Operand0 + Sf0) = R0 + R1 + R2;
		break;
	case EInstruction::LOAD:
		*(long*)(Address + Ins.Operand0) = Ins.OperandLong;
		break;
	case EInstruction::LOADA:
		break;
	case EInstruction::LOADR:
		break;
	case EInstruction::STOREA:
		break;
	case EInstruction::STORER:
		break;
	case EInstruction::JMP:
		ProgramCounter = Ins.Operand0 - 1;
		break;
	case EInstruction::IFJMP:
		if (*(bool*)(Address + Ins.Operand1))ProgramCounter = Ins.Operand0 - 1;
		break;
	case EInstruction::IFNJMP:
		if (!*(bool*)(Address + Ins.Operand1))ProgramCounter = Ins.Operand0 - 1;
		break;
	case EInstruction::CALL:
		*(long*)(Memory + StackPointer) = ProgramCounter;
		StackPointer -= 4;
		ProgramCounter = Ins.Operand0 - 1;
		break;
	case EInstruction::ENTRY:
		*(long*)(Memory + StackPointer) = BasePointer;
		BasePointer = StackPointer;
		StackPointer -= Ins.Operand0;
		break;
	case EInstruction::RETURN:
		StackPointer = BasePointer;
		BasePointer = *(long*)(Memory + StackPointer);
		ProgramCounter = *(long*)(Memory + StackPointer + 4);
		StackPointer += 8;
		break;
	case EInstruction::MOVE:
		memcpy(Address + Ins.Operand0, Address + Ins.Operand1, Ins.Operand2);
		break;
	case EInstruction::END:
		return true;
	case EInstruction::PUSHV:
		Model.Vertexes.push_back(*((float3*)Address + Ins.Operand0));
		break;
	case EInstruction::PUSHI:
		Shape.Triangles.push_back(*(long*)(Address + Ins.Operand0));
		Shape.Triangles.push_back(*(long*)(Address + Ins.Operand1));
		Shape.Triangles.push_back(*(long*)(Address + Ins.Operand2));
		break;
	case EInstruction::PUSHS:
		Model.Shapes.push_back(Shape);
		break;
	case EInstruction::PUSHSI:
		ShapeIndex.Index = *(long*)(Address + Ins.Operand0);
		ShapeIndex.Position = *((float3*)Address + Ins.Operand1);
		ShapeIndex.Rotation = *((float3*)Address + Ins.Operand2);
		Model.ShapeIndex.push_back(ShapeIndex);
		break;
	default:
		assert(false);
		break;
	}
	return true;
}

void FVirtualMachine::Run(const vector<SInstruction>& Instructions)
{
	BasePointer = 0, StackPointer = 0;
	for(ProgramCounter = 0; ; ProgramCounter++)
	{
		if(!Instruct(Instructions[ProgramCounter]))return;
	}
}