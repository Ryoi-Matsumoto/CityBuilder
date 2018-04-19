#pragma once
#include "../../FrontEnd/Public/FrontEnd.h"
#include "../../Shape.h"

class FVirtualMachine
{
public:
	const SIntermediate* Intermediate;
	long FramePointer;
	long ProgramCounter;
	uint8 CallStack[512];
	uint8 MainStack[100000];
	uint CallStackOffset;
	SModelObject Model;
	SShapeObject Shape;

public:
	FVirtualMachine() {}

	bool Instruct(const SInstruction& Instruction);
	void Run(const SIntermediate& Intermediate);
};