#include "FrontEnd\Public\FrontEnd.h"
#include "BackEnd\Public\VirtualMachine.h"

void UnitTestProcedureFrontEnd()
{
	string Source;
	SIntermediate Result;
	FVirtualMachine VM;

	Source = "int add(int a, int b) => return a + b";
	Result = GenerateIntermediate(Source);

	Source = "shape rectangle {int3 indexes} def startup:rectangle(int a, int b) => a = 4";
	Result = GenerateIntermediate(Source);
	VM.Run(Result);
}