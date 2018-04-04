#pragma once
#include "../../Intermediate.h"

class FStatementBlock;

struct SLiteral
{
	bool IsValid;
	union
	{
		float Real;
		int Int;
		float2 Real2;
		int2 Int2;
		float3 Real3;
		int3 Int3;
		bool Bool;
	};

	SLiteral() : IsValid(false) {}
	SLiteral(bool Bool) : Bool(Bool), IsValid(true) {}
	SLiteral(float Float) : Real(Float), IsValid(true) {}
	SLiteral(int Int) : Int(Int), IsValid(true) {}
	SLiteral(float2 Float2) : Real2(Float2), IsValid(true) {}
	SLiteral(int2 Int2) : Int2(Int2), IsValid(true) {}
	SLiteral(float3 Float3) : Real3(Float3), IsValid(true) {}
	SLiteral(int3 Int3) : Int3(Int3), IsValid(true) {}
};

struct SVariable
{
	EValueType Type;
	regptr Register;
	SLiteral Literal;
	uint Size;

	SVariable() {}

	SVariable(EValueType Type, regptr Register) : Type(Type), Register(Register) {}
};

struct SFunction
{
	EValueType ReturnType;
	vector<EValueType> ArgumentTypes;
	FStatementBlock* Block;
	regptr Label;

	SFunction() {}

	SFunction(EValueType ReturnType, vector<EValueType> ArgumentTypes, FStatementBlock* Block, regptr Label)
		: ReturnType(ReturnType), ArgumentTypes(ArgumentTypes), Block(Block), Label(Label) {}
};

enum class EProcedureType
{
	Tag, Value
};

struct SShape
{
	vector<pair<EValueType, string>> Members;
	vector<FStatementBlock*> Converters;
	uint Size;
};

struct SProcedure
{
	SShape* TargetShape;
	EProcedureType Type;
	FStatementBlock* Block;
	regptr Label;
	vector<EValueType> ArgumentTypes;

	//union
	//{
		vector<pair<string, SShape*>> ForkTagYield;
		//struct
		//{
			SShape* ForkValueYield;
			EValueType ForkValueType;
		//};
	//};

	SProcedure() {}

	SProcedure(SShape* TargetShape, FStatementBlock* Block, regptr Label, vector<EValueType> ArgumentTypes)
		: Type(EProcedureType::Tag), ArgumentTypes(ArgumentTypes), TargetShape(TargetShape), Block(Block), Label(Label), ForkTagYield(ForkTagYield) {}
};

#define FScopeFind(N) auto Itr = N##s.find(Name); return Itr != N##s.end() ? &Itr->second : (Parent ? Parent->Find##N(Name) : nullptr)
#define FScopeAdd(N) auto Pair = N##s.try_emplace(Args...); return Pair.second ? &(Pair.first->second) : nullptr;

struct FScope
{
private:
	FScope* Parent;
	map<string, SVariable> Variables;
	map<string, SFunction> Functions;
	map<string, SProcedure> Procedures;
	map<string, SShape> Shapes;
	regptr RegisterSize;

public:
	FScope(FScope* Parent) : Parent(Parent), RegisterSize(0) {}
	FScope(FScope* Parent, regptr RegisterOffset) : Parent(Parent), RegisterSize(RegisterOffset) {}

	void ReserveRegister(regptr Size) { RegisterSize += Size; }
	void ReleaseRegister(regptr Size) { RegisterSize -= Size; }
	regptr GetStackPointer() const { return RegisterSize + (Parent ? Parent->GetStackPointer() : 0); }
	SVariable* FindVariable(string Name) { FScopeFind(Variable); }
	const SFunction* FindFunction(string Name) const { FScopeFind(Function); }
	const SProcedure* FindProcedure(string Name) const { FScopeFind(Procedure); }
	SShape* FindShape(string Name) { FScopeFind(Shape); }
	
	template<typename... TArgs> 
	bool AddVariable(TArgs&&... Args)
	{
		auto Pair = Variables.try_emplace(Args...);
		if (Pair.second)RegisterSize += Pair.first->second.Size;
		return Pair.second;
	}
	template<typename... TArgs> SFunction* AddFunction(TArgs&&... Args) { FScopeAdd(Function) }
	template<typename... TArgs> SProcedure* AddProcedure(TArgs&&... Args) { FScopeAdd(Procedure) }
	template<typename... TArgs> SShape* AddShape(TArgs&&... Args) { FScopeAdd(Shape) }
};

struct FRoutine
{
private:
	vector<SInstruction> Instructions;
	vector<pair<string, SShape*>> ForkTagYield;
	SProcedure* CurrentProcedure;

public:
	FRoutine(SProcedure* CurrentProcedure) : CurrentProcedure(CurrentProcedure) {}

	void AddInstruction(EInstruction Code, EOperationType Type, regptr Operand0, regptr Operand1, regptr Operand2)
	{ 
		return Instructions.emplace_back(SInstruction(Code, Type, Operand0, Operand1, Operand2));
	}
	void StoreRoutine(const FRoutine& Routine) 
	{
		copy(Routine.Instructions.begin(), Routine.Instructions.end(), back_inserter(Instructions));
	}
	vector<SInstruction>& GetInstructions() { return Instructions; }
	SProcedure* GetCurrentProcedure() const { return CurrentProcedure; }
	void AddTagYield(string Tag, SShape* Shape) { ForkTagYield.push_back(pair<string, SShape*>(Tag, Shape)); }
};

class FIntermediate
{
private:
	//FRoutine MainRoutine;
	vector<long> Data;
	vector<SError> Errors;
	uint LabelCount;

public:
	FIntermediate() : LabelCount(0) {}

	uint IssueLabel() { LabelCount++; return LabelCount - 1; }
	void AddData(long InData) { Data.push_back(InData); }
	template<typename... TArgs> void AddError(TArgs&&... Args) { Errors.emplace_back(Args...); }
	regptr GetDataIndex() const { return Data.size(); }
};

struct SCompilingData
{
	FIntermediate* Intermediate;
	FRoutine* Routine;
	FScope* Scope;
};

enum class ECompilingMode
{
	Od, O2
};

/*
struct SValue
{
};

struct SForkParam
{
	string Name;
};

class FShape
{
	vector<SQuadrangleIndex> Indiecs;
	vector<FShape> Children;

public:
	// FShape** Extrude(float hight, FCallForkCase* forkCase);
	vector<pair<string, function<void(FShape)>>> Functions;
};

class FStartupShape : FShape
{
private:
	vector<float3> Vertices;
	SMesh Mesh;

public:
	FStartupShape(SQuadrangle3 Quadrangle)
	{
		Mesh.NumVertex = 4;
		Mesh.Vertices = new SVertex[4];
		for (int i = 0; i < 4; i++)
		{
			SVertex Vertex;
			Vertex.Pos = Quadrangle.Points[i];
			Mesh.Vertices[i] = Vertex;
		}

		Mesh.NumIndex = 4;
		Mesh.Indices = new uint[6]{ 0,1,2,1,3,2 };
	}

	FStartupShape(vector<SPairedCurve> Curves)
	{		
			Mesh.Vertices = new SVertex[Curves.size()*2];
			Mesh.NumVertex = Mesh.Vertices.size();

			for (int i = 0; i < Curves.size(); i++)
			{
			SVertex Vertex0;
			Vertex0.Pos = Curves[i].Pairs[0];

			SVertex Vertex1;
			Vertex1.Pos = Curves[i].Pairs[1];

			Mesh.Vertices[i*2] = Vertex0
			Mesh.Vertices[i*2+1] = Vertex1;
			}

			Mesh.NumIndex = (Curves.size() - 1) * 6;
			Mesh.Indices = new uint[Mesh.NumIndex];

			for (int i = 0; i < Curves.size()-1; i++)
			{
			Mesh.Indices[i*6] = i*2;
			Mesh.Indices[i*6] = i*2+1;
			Mesh.Indices[i*6] = i*2+2;

			Mesh.Indices[i*6] = i*2+1;
			Mesh.Indices[i*6] = i*2+3;
			Mesh.Indices[i*6] = i*2+2;
			}
	}

	SMesh ToMesh()
	{
		return Mesh;
	}
};

*/