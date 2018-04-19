#pragma once

extern void Rendering2DTest();
extern void ParserTest();
extern void JsonBenchmark();
extern void ProcedualTest();

inline void AllTest()
{
	ParserTest();
	//JsonBenchmark();
	ProcedualTest();
}