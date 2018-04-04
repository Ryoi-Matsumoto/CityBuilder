#pragma once

using regptr = uint16;

enum class EValueType
{
	Error, Int, Int2, Int3, Int3x4, Int3x8, Real, Real2, Real3, Real3x4, Real3x8, Boolean
};

enum class EInstruction : uint8
{
	LABEL,
	ADD, SUB, MUL, DIV, MOD, MNS,
	ICE, ICNE, ICG, ICL, ICGE, ICLE,
	FCE, FCNE, FCG, FCL, FCGE, FCLE,
	LN, LAND, LOR,
	SIN, COS, TAN,
	VNORM, VCROS, VDOT,
	EXTRACT, INSERT,
	LOAD, LOADA, LOADR, STOREA, STORER, STORERA,
	JMP, IFJMP, IFNJMP, MOVE, CALL, ENTRY, RETURN, END,
	PUSHV, PUSHI, PUSHS, PUSHSI, ERROR
};

enum class EOperationType : uint8
{
	II, I2I2, I2I, I3I, I3I3, I3x4I3x4, I3x4I, I3x8I3x8, I3x8I, I3x4I3, I3x8I3, I3x8I3x4,
	RR, R2R2, R2R, R3R, R3R3, R3x4R3x4, R3x4R, R3x8R3x8, R3x8R, R3x4R3, R3x8R3, R3x8R3x4,
	BB, None, Error
};

struct SInstruction
{
	EInstruction Code;	
	EOperationType Type;
	regptr Operand0;
	regptr Operand1;
	regptr Operand2;

	bool Literal;

	SInstruction(EInstruction Code, EOperationType Type, regptr Operand0, regptr Operand1, regptr Operand2)
		: Code(Code), Type(Type), Operand0(Operand0), Operand1(Operand1), Operand2(Operand2) {}
};