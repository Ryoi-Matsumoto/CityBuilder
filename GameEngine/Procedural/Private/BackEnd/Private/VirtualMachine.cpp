#include "../Public/VirtualMachine.h"
#include <intrin.h>

/*
予めサイズと型を決定する
storea, storer 後から用途を決定する
loada, loadr, load 前からサイズ, 型,　用途を決定する

loadaとstorerが競合したとき、途中で代入時にfreeするからそのときは両方レジスタが使える

storea レジスタ, インデックス, 種類
*/

bool FVirtualMachine::Instruct(const SInstruction& Ins)
{
	auto Address = MainStack + FramePointer;
	uint Size;

	switch (Ins.Operation.Code)
	{
	case EInstruction::LABEL:
		break;
	case EInstruction::ADD:
	case EInstruction::SUB:
	case EInstruction::MUL:
	case EInstruction::DIV:
	case EInstruction::MOD:
	case EInstruction::AND:
	case EInstruction::OR:
	case EInstruction::DOT:
	case EInstruction::SQRT:
	case EInstruction::CMPE:
	case EInstruction::CMPNE:
	case EInstruction::CMPG:
	case EInstruction::CMPL:
	case EInstruction::CMPGE:
	case EInstruction::CMPLE:
		Ins.Type.Calculate(Ins.Operation, Address + Ins.Operand0, Address + Ins.Operand1, Address + Ins.Operand2);
		break;
	case EInstruction::MNS:
	case EInstruction::NOT:
		Ins.Type.Calculate(Ins.Operation, Address + Ins.Operand0, Address + Ins.Operand1);
		break;
	case EInstruction::CAST:
		Ins.Type.Convert(Ins.Type.ValueType, Address + Ins.Operand0, Address + Ins.Operand1);
		break;
	case EInstruction::EXTRACT:
		Ins.Type.Extract(Address + Ins.Operand0, Address + Ins.Operand1, Ins.Operand2);
		break;
	case EInstruction::INSERT:
		Ins.Type.Insert(Address + Ins.Operand0, Address + Ins.Operand1, Ins.Operand2);
		break;
	case EInstruction::LOAD:
		memcpy(Address + Ins.Operand0, &Intermediate->Data[0] + Ins.Operand1, Ins.Type.GetSize());
		break;
	case EInstruction::STOREA:
	case EInstruction::STORER:
		Size = Ins.Type.GetSize();
		CallStackOffset += Size;
		memcpy(CallStack + CallStackOffset, Address + Ins.Operand0, Size);
		break;
	case EInstruction::LOADA:
	case EInstruction::LOADR:
		Size = Ins.Type.GetSize();
		CallStackOffset += Size;
		memcpy(Address + Ins.Operand0, CallStack + CallStackOffset, Size);
		break;
	case EInstruction::STORERA:

		break;
	case EInstruction::JMP:
		ProgramCounter = Intermediate->Labels[Ins.Operand0] - 1;
		break;
	case EInstruction::IFJMP:
		if (*(bool*)(Address + Ins.Operand1))
			ProgramCounter = Intermediate->Labels[Ins.Operand0] - 1;
		break;
	case EInstruction::IFNJMP:
		if (!*(bool*)(Address + Ins.Operand1))
			ProgramCounter = Intermediate->Labels[Ins.Operand0] - 1;
		break;
	case EInstruction::MOVE:
		memcpy(Address + Ins.Operand0, Address + Ins.Operand1, Ins.Type.GetSize());
		break;
	case EInstruction::CALL:
		Size = *(long*)(Address + Ins.Operand1);
		*(long*)(Address + Size) = ProgramCounter + 1;
		*(long*)(Address + Size + sizeof(long)) = FramePointer;
		ProgramCounter = Intermediate->Labels[Ins.Operand0];
		FramePointer += Size + sizeof(long) * 2;
		CallStackOffset = 0;

		ProgramCounter--;
		break;
	case EInstruction::ENTRY:
		break;
	case EInstruction::RETURN:
		ProgramCounter = *(long*)(Address - sizeof(long) * 2);
		FramePointer = *(long*)(Address - sizeof(long));
		CallStackOffset = 0;

		ProgramCounter--;
		break;
	case EInstruction::END:
		break;
	case EInstruction::PUSHV:
		Model.Vertexes.push_back(*((float3*)Address + Ins.Operand0));
		break;
	case EInstruction::PUSHI:
		Shape.Indexes.push_back(*(array<uint32, 3>*)(Address + Ins.Operand0));
		break;
	case EInstruction::PUSHS:
		Model.Shapes.push_back(Shape);
		break;
	case EInstruction::ERROR:
		break;
	default:
		assert(false);
	}
	return true;
}

void FVirtualMachine::Run(const SIntermediate& InIntermediate)
{
	Intermediate = &InIntermediate;

	FramePointer = 0, CallStackOffset = 0;
	for(ProgramCounter = 0; ; ProgramCounter++)
	{
		if(!Instruct(Intermediate->Instructions[ProgramCounter]))return;
	}
}