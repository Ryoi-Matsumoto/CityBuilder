#pragma once
#include "..\..\Intermediate.h"
#include "..\..\Shape.h"

class FVirtualMachine
{
public:
	long BasePointer;
	long StackPointer;
	long ProgramCounter;
	char Memory[100000];
	SModel Model;
	SShape Shape;

public:
	bool Instruct(const SInstruction& Instruction);
	void Run(const vector<SInstruction>& Instructions);
};