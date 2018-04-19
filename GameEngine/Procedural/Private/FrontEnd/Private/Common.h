#pragma once
#include "../../Intermediate.h"

class FStatementBlock;

struct SLiteral
{
	vector<int> Vector;
	union
	{
		float Real;
		int Int;
		bool Bool;
	};

	SLiteral() {}
	SLiteral(bool Bool) : Bool(Bool) {}
	SLiteral(float Real) : Real(Real) {}
	SLiteral(int Int) : Int(Int) {}

	static SLiteral Empty(uint Size)
	{
		SLiteral Literal;
		if (Size > 1)Literal.Vector.resize(Size);
		return Literal;
	}

	void* GetPointer() const { return Vector.size() == 0 ? (void*)&Int : (void*)&Vector[0]; }
};

struct SVariable
{
	SType Type;
	regptr Register;
	SLiteral Literal;
	bool IsLiteralVaild;

	SVariable() : IsLiteralVaild(false) {}
	SVariable(SType Type, regptr Register) : IsLiteralVaild(false), Type(Type), Register(Register) {}
};

struct SShape
{
	vector<pair<SType, string>> Members;
	vector<FStatementBlock*> Converters;
	uint Size;


	SShape(vector<pair<SType, string>> Members) : Members(Members) {}
};

struct SCompilingData;

struct SFunction
{
	using TGenerateCode = regptr (*)(const SCompilingData& Data, const vector<regptr>& Arguments);

	bool IsBuiltInNotDefine;

	SType ReturnType;
	vector<SType> ArgumentTypes;

	union
	{
		struct
		{
			const FStatementBlock* Block;
			regptr Label;
		};
		TGenerateCode GenerateCode;
	};

	SFunction() {}
	SFunction(SType ReturnType, vector<SType> ArgumentTypes, const FStatementBlock* Block, regptr Label)
		: ReturnType(ReturnType), ArgumentTypes(ArgumentTypes), Block(Block), Label(Label) {}
	SFunction(SType ReturnType, vector<SType> ArgumentTypes, TGenerateCode GenerateCode)
		: ReturnType(ReturnType), ArgumentTypes(ArgumentTypes), GenerateCode(GenerateCode) {}
};

struct SProcedure
{
	using TGenerateCode = void (*)(const SCompilingData& Data, const vector<regptr>& Arguments);

	bool IsTagNotValue;
	bool IsBuiltInNotDefine;

	const SShape* TargetShape;
	vector<SType> ArgumentTypes;

	vector<pair<string, const SShape*>> ForkTagYield;
	const SShape* ForkValueYield;
	SType ForkValueType;
	
	union
	{
		struct
		{
			const FStatementBlock* Block;
			regptr Label;
		};
		TGenerateCode GenerateCode;
	};

	SProcedure() {}
	SProcedure(bool IsTagNotValue, const SShape* TargetShape, const FStatementBlock* Block, vector<SType> ArgumentTypes, SType ForkValueType, regptr Label)
		: IsBuiltInNotDefine(false), IsTagNotValue(IsTagNotValue), TargetShape(TargetShape), Block(Block), ArgumentTypes(ArgumentTypes), ForkValueType(ForkValueType), Label(Label) {}
	SProcedure(vector<SType> ArgumentTypes, TGenerateCode GenerateCode)
		: IsBuiltInNotDefine(true), GenerateCode(GenerateCode), ArgumentTypes(ArgumentTypes) {}
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
		if (Pair.second)RegisterSize += Pair.first->second.Type.GetSize();
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
	SProcedure* CurrentProcedure;

public:
	FRoutine(SProcedure* CurrentProcedure) : CurrentProcedure(CurrentProcedure) {}

	void AddInstruction(EInstruction Code, regptr Operand0, regptr Operand1, regptr Operand2)
	{
		return Instructions.emplace_back(SInstruction(Code, SType(), Operand0, Operand1, Operand2));
	}
	void AddInstruction(EInstruction Code, SType Type, regptr Operand0, regptr Operand1, regptr Operand2)
	{ 
		return Instructions.emplace_back(SInstruction(Code, Type, Operand0, Operand1, Operand2));
	}	
	void AddInstruction(SOperation Operation, SType Type, regptr Operand0, regptr Operand1, regptr Operand2)
	{
		return Instructions.emplace_back(SInstruction(Operation, Type, Operand0, Operand1, Operand2));
	}
	void StoreRoutine(const FRoutine& Routine) 
	{
		copy(Routine.Instructions.begin(), Routine.Instructions.end(), back_inserter(Instructions));
	}

	vector<SInstruction> GetInstructions() { return Instructions; }
	SProcedure* GetCurrentProcedure() const { return CurrentProcedure; }
	void AddTagYield(string Tag, const SShape* Shape) { CurrentProcedure->ForkTagYield.push_back(pair<string, const SShape*>(Tag, Shape)); }
};

class FProgram
{
private:
	//FRoutine MainRoutine;
	vector<uint8> Data;
	vector<SError> Errors;
	uint LabelCount;

public:
	FProgram() : LabelCount(0) {}

	vector<uint8>* GetData() { return &Data; }
	vector<SError>* GetErrors() { return &Errors; }
	uint GetLabelCount() const { return LabelCount; }
	uint IssueLabel() { LabelCount++; return LabelCount - 1; }
	void AddData(long InData) { Data.push_back(InData); }
	template<typename... TArgs> void AddError(TArgs&&... Args) { Errors.emplace_back(Args...); }
	regptr GetDataIndex() const { return Data.size(); }
};

struct SCompilingData
{
	FProgram* Program;
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