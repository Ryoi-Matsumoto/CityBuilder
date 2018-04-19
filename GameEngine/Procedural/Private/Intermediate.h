#pragma once
#include "Type.h"

enum class EInstruction : uint8
{
	LABEL,
	ADD, SUB, MUL, DIV, MOD, MNS,
	NOT, AND, OR,
	DOT, SQRT,
	CMPE, CMPNE, CMPG, CMPL, CMPGE, CMPLE,
	CAST,
	EXTRACT, INSERT,
	LOAD, LOADA, LOADR, STOREA, STORER, STORERA,
	JMP, IFJMP, IFNJMP, MOVE,
	CALL, ENTRY, RETURN, END,
	PUSHV, PUSHI, PUSHS, PUSHSI, ERROR
};

struct SOperation
{
	EInstruction Code;
	bool IsBloadcast;

	SOperation(EInstruction Code) : Code(Code), IsBloadcast(false) {}
	SOperation(EInstruction Code, bool IsBloadcast) : Code(Code), IsBloadcast(IsBloadcast) {}
};

struct SInstruction
{
	SOperation Operation;
	SType Type;
	regptr Operand0;
	regptr Operand1;
	regptr Operand2;
	
	SInstruction(EInstruction Code, SType Type, regptr Operand0, regptr Operand1, regptr Operand2)
		: Operation(Code), Type(Type), Operand0(Operand0), Operand1(Operand1), Operand2(Operand2) {}

	SInstruction(SOperation Operation, SType Type, regptr Operand0, regptr Operand1, regptr Operand2)
		: Operation(Operation), Type(Type), Operand0(Operand0), Operand1(Operand1), Operand2(Operand2) {}
};