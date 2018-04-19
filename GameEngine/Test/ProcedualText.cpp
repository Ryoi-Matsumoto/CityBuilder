#include "Test.h"
#include <time.h>
#include <Windows.h>
#include <sstream>

int fib(int n) { if (n <= 1) { return 1; }return fib(n - 1) + fib(n - 2); }
int m() { int n = 0; int f = 0; while (n < 38) { f = fib(n); n = n + 1; } return f; }
void UnitTestProcedureFrontEnd();
void ProcedualTest()
{
	UnitTestProcedureFrontEnd();
	/*
	Compiler.Compile
	(
		"int fib(int n){if(n<=1){return 1}return fib(n-1)+fib(n-2)}int main(){int n=0 int f=0 while(n<38){f=fib(n) n=n+1} return f}"
		//"int fib(int n){if(n<=1){return 1}return fib(n-1)+fib(n-2)}int main(){return fib(40)}"
		//"int main(){int n=0 while(n<38){n=n+1} return n}"
	);
	auto start = clock();
	double ResultCpp= m();
	double TimeCpp = (double)(clock() - start) / CLOCKS_PER_SEC;
	
	start = clock();
	Compiler.Run();
	auto TimeScript = (double)(clock() - start) / CLOCKS_PER_SEC;

	long Array[100];
	memcpy(Array, Compiler.Result, sizeof(long) * 100);
	double ResultScript = Array[0]; // IADD 12, 20, 20 bug func + func


	start = clock();
	for (uint i = 0; i < 1; i++)
	{
		Compiler.Run();
	}
	//time = (double)(clock() - start) / CLOCKS_PER_SEC;
	ostringstream s;
	s << "Cpp:" << TimeCpp << " Script:" << TimeScript << "Cpp:" << ResultCpp << " Script:" << ResultScript;
	MessageBoxA(0, &s.str()[0], "", 0);
	//FFileManager::WriteTextFile(L"aa.txt", (wchar_t*)&time);
	*/
	// 0   1   2        3   4      5  
	// bpc bfp return
	//         bpc      bfp return 
	//                      a      b 
	// 0   4   8        12  16     20 
	//         0        4   8      12   16  20 24 28
}