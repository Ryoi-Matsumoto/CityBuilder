#include "Compiler.h"
#include "xbyak\xbyak.h"
#undef ERROR

enum class EVirtualRegisterUsage
{
	Argument, Local, Literal, Return
};

enum class ESIMDType
{
	AVX, AVX2
};

struct SOperandSet
{
	SOperandSet() {}

	SOperandSet(Xbyak::Reg32&& SingleInt, uint OperandIndex) : SingleInt(move(SingleInt)), OperandIndex(OperandIndex) {}

	union
	{
		Xbyak::Reg32 SingleInt;
		Xbyak::Xmm SingleVector;
		Xbyak::Xmm MultiVector[6];
	};

	union
	{
		uint OperandIndex;
		uint OperandIndexes[6];
	};
};

struct SRegisterType
{
	bool IsFloat;
	uint Count;

	SRegisterType() {}

	SRegisterType(bool IsFloat, uint Count) : IsFloat(IsFloat), Count(Count) {}
};

struct SVirtualRegister
{
	regptr ExpirationProgramCount;
	EVirtualRegisterUsage Usage;
	bool IsAllocated;
	SOperandSet Operand;
	SRegisterType Type;
};

class SCallingConvention
{
	uint ArgumentCount;
	uint ReturnCount;
	Xbyak::Operand Arguments[16];
	Xbyak::Operand Returns[16];
};

class FRegisterTable
{
private:
	static const uint VRegisterCount = 512;
	static const uint GPRegisterCount = 16;
	static const uint SIMDRegisterCount = 16;
	static const uint x87RegisterCount = 16;
	SVirtualRegister VRegisters[VRegisterCount];
	bool GPRegisters[GPRegisterCount];
	bool SIMDRegisters[SIMDRegisterCount];
	bool x87Registers[x87RegisterCount];
	bool IsSupportingAVX2;
	Xbyak::CodeGenerator gen;

private:
	SOperandSet Allocate(SRegisterType Type)
	{
		if(!Type.IsFloat && Type.Count == 1)
		{
			for (uint i = 0; i < GPRegisterCount; i++)
			{
				if (!GPRegisters[i])
				{
					GPRegisters[i] = true;
					return SOperandSet(Xbyak::Reg32(i), i);
				}
			}
			assert(false);
		}
		else
		{
			uint RegisterSize = IsSupportingAVX2 ? 256 : 128;
			uint RegisterCount = Type.Count / (RegisterSize / 32);
			SOperandSet OperandSet;
			for (uint i = 0; i < SIMDRegisterCount; i++)
			{
				if (!SIMDRegisters[i])
				{
					SIMDRegisters[i] = true;
					RegisterCount--;

					if(IsSupportingAVX2)OperandSet.MultiVector[RegisterCount] = Xbyak::Ymm(i);
					else OperandSet.MultiVector[RegisterCount] = Xbyak::Xmm(i);

					if (RegisterCount == 0)break;
				}
			}
			if (RegisterCount > 0)assert(false);
			return OperandSet;
		}
	}

	void Free(regptr VirtualRegister)
	{
		auto& VRegister = VRegisters[VirtualRegister];
		assert(VRegister.IsAllocated);
		VRegister.IsAllocated = false;

		if (!VRegister.Type.IsFloat && VRegister.Type.Count == 1)
		{
			GPRegisters[VRegister.Operand.OperandIndex] = false;
		}
		else
		{
			for (uint i = 0; VRegister.Type.Count; i++)
			{
				SIMDRegisters[VRegister.Operand.OperandIndexes[i]] = false;
			}
		}
	}

	void Move(regptr VirtualDest, regptr VirtualSource)
	{
		auto Type = VRegisters[VirtualDest].Type;
		auto Dest = VRegisters[VirtualDest].Operand;
		auto Source = VRegisters[VirtualSource].Operand;

		if (!Type.IsFloat && Type.Count == 1)
		{
			gen.mov(Dest.SingleInt, Source.SingleInt);
		}
		else
		{
			for (uint i = 0; i < Type.Count; i++)
			{
				gen.vmovaps(Dest.MultiVector[i], Source.MultiVector[i]);
			}
		}
	}

	bool IsNeedless(regptr VirtualRegister, regptr ProgramCount)
	{
		return VRegisters[VirtualRegister].ExpirationProgramCount == ProgramCount;
	}

public:
	FRegisterTable()
	{
		for (uint i = 0; i < VRegisterCount; i++)
		{
			VRegisters[i].IsAllocated = false;
		}
	}

	void PrimaryRead(regptr VirtualRegister, regptr ProgramCount)
	{
		VRegisters[VirtualRegister].ExpirationProgramCount = ProgramCount;
	}

	void PrimarySetUsage(regptr VirtualRegister, EVirtualRegisterUsage Usage, SRegisterType Type)
	{
		VRegisters[VirtualRegister].Usage = Usage;
		VRegisters[VirtualRegister].Type = Type;
	}

	SOperandSet Use(regptr Used, regptr ProgramCount)
	{
		assert(VRegisters[Used].IsAllocated);
		if (IsNeedless(Used, ProgramCount))Free(Used);
		return VRegisters[Used].Operand;
	}

	SOperandSet Assign(regptr Assigned, SRegisterType Type)
	{
		auto& VRegister = VRegisters[Assigned];
		if (!VRegister.IsAllocated)
		{
			VRegister.Operand = Allocate(Type);
			VRegister.IsAllocated = true;
		}
		return VRegister.Operand;
	}

	SOperandSet AssignAndMove(regptr Assigned, regptr Source, regptr ProgramCount, SRegisterType Type)
	{
		assert(VRegisters[Assigned].IsAllocated);
		assert(VRegisters[Source].IsAllocated);

		if (IsNeedless(Source, ProgramCount))
		{
			VRegisters[Assigned] = VRegisters[Source];
			VRegisters[Source].IsAllocated = false;
			return VRegisters[Assigned].Operand;
		}
		if (Assigned == Source)
		{
			return VRegisters[Source].Operand;
		}
		auto Result = Assign(Assigned, Type);
		Move(Assigned, Source);
		return Result;
	}

	Xbyak::Ymm GetTemporarySIMDRegister()
	{
		for (uint i = 0; i < SIMDRegisterCount; i++)
		{
			if (!SIMDRegisters[i])return Xbyak::Ymm(i);
		}
		assert(false);
	}

	array<SOperandSet, 2> Get2Operands(SRegisterType Type, regptr Operand0, regptr Operand1, regptr Operand2, uint ProgramCount)
	{
		array<SOperandSet, 2> Result;
		Result[0] = AssignAndMove(Operand0, Operand1, ProgramCount, Type);
		Result[1] = Use(Operand2, ProgramCount);
		return Result;
	}

	array<SOperandSet, 3> Get3Operands(SRegisterType Type, regptr Operand0, regptr Operand1, regptr Operand2, uint ProgramCount)
	{
		array<SOperandSet, 3> Result;
		Result[0] = Assign(Operand0, Type);
		Result[1] = Use(Operand1, ProgramCount);
		Result[2] = Use(Operand2, ProgramCount);
		return Result;
	}
};

SRegisterType GetDestRegisterType(EOperationType Type)
{
	switch (Type)
	{
	case EOperationType::II:return SRegisterType(false, 1);
	case EOperationType::I2I2:return SRegisterType(false, 2);
	case EOperationType::I2I:return SRegisterType(false, 2);
	case EOperationType::I3I:return SRegisterType(false, 3);
	case EOperationType::I3I3:return SRegisterType(false, 3);
	case EOperationType::I3x4I3x4:return SRegisterType(false, 12);
	case EOperationType::I3x4I:return SRegisterType(false, 12);
	case EOperationType::I3x8I3x8:return SRegisterType(false, 24);
	case EOperationType::I3x8I:return SRegisterType(false, 24);
	case EOperationType::I3x4I3:return SRegisterType(false, 12);
	case EOperationType::I3x8I3:return SRegisterType(false, 24);
	case EOperationType::I3x8I3x4:return SRegisterType(false, 24);
	case EOperationType::RR:return SRegisterType(true, 1);
	case EOperationType::R2R2:return SRegisterType(true, 2);
	case EOperationType::R2R:return SRegisterType(true, 2);
	case EOperationType::R3R:return SRegisterType(true, 3);
	case EOperationType::R3R3:return SRegisterType(true, 3);
	case EOperationType::R3x4R3x4:return SRegisterType(true, 12);
	case EOperationType::R3x4R:return SRegisterType(true, 12);
	case EOperationType::R3x8R3x8:return SRegisterType(true, 24);
	case EOperationType::R3x8R:return SRegisterType(true, 24);
	case EOperationType::R3x4R3:return SRegisterType(true, 12);
	case EOperationType::R3x8R3:return SRegisterType(true, 24);
	case EOperationType::R3x8R3x4:return SRegisterType(true, 24);
	case EOperationType::BB:return SRegisterType(true, 1);
	default:assert(false); break;
	}
}

#define VECTOR_CALC(CALC, BLCST, MOVH) \
case EOperationType::II: \
	gen.CALC(Operands[0].SingleVector, Operands[1].SingleVector, Operands[2].SingleVector); \
	break; \
case EOperationType::I2I2: \
	/* 0bit              256bit */ \
	/* x y na na na na na na    */ \
	gen.CALC(Operands[0].SingleVector, Operands[1].SingleVector, Operands[2].SingleVector); \
	break; \
case EOperationType::I2I: \
	gen.BLCST(Operands[2].SingleVector, Operands[2].SingleVector); \
	gen.CALC(Operands[0].SingleVector, Operands[1].SingleVector, Operands[2].SingleVector); \
	break; \
case EOperationType::I3I3: \
	/* x y z na na na na na */ \
	gen.CALC(Operands[0].SingleVector, Operands[1].SingleVector, Operands[2].SingleVector); \
	break; \
case EOperationType::I3I: \
	gen.BLCST(Operands[2].SingleVector, Operands[2].SingleVector); \
	gen.CALC(Operands[0].SingleVector, Operands[1].SingleVector, Operands[2].SingleVector); \
	break; \
case EOperationType::I3x4I3x4: \
	/* ymm0 ax ay az na bx by bz na */ \
	/* ymm1 cx cy cz na dx dy dz na */ \
	gen.CALC(Operands[0].MultiVector[0], Operands[1].MultiVector[0], Operands[2].MultiVector[0]); \
	gen.CALC(Operands[0].MultiVector[1], Operands[1].MultiVector[1], Operands[2].MultiVector[1]); \
	break; \
case EOperationType::I3x4I: \
	gen.BLCST(Operands[2].SingleVector, Operands[2].SingleVector); \
	gen.CALC(Operands[0].MultiVector[0], Operands[1].MultiVector[0], Operands[2].SingleVector); \
	gen.CALC(Operands[0].MultiVector[1], Operands[1].MultiVector[1], Operands[2].SingleVector); \
	break; \
case EOperationType::I3x4I3: \
	gen.MOVH(Operands[2].SingleVector, Operands[2].SingleVector); \
	gen.CALC(Operands[0].MultiVector[0], Operands[1].MultiVector[0], Operands[2].SingleVector); \
	gen.CALC(Operands[0].MultiVector[1], Operands[1].MultiVector[1], Operands[2].SingleVector); \
	break; \
case EOperationType::I3x8I3x8: \
	/* ymm0 ax ay az na bx by bz na */ \
	/* ymm1 cx cy cz na dx dy dz na */ \
	/* ymm2 ex ey ez na fx fy fz na */ \
	/* ymm3 gx gy gz na hx hy hz na */ \
	gen.CALC(Operands[0].MultiVector[0], Operands[1].MultiVector[0], Operands[2].MultiVector[0]); \
	gen.CALC(Operands[0].MultiVector[1], Operands[1].MultiVector[1], Operands[2].MultiVector[1]); \
	gen.CALC(Operands[0].MultiVector[2], Operands[1].MultiVector[2], Operands[2].MultiVector[2]); \
	gen.CALC(Operands[0].MultiVector[3], Operands[1].MultiVector[3], Operands[2].MultiVector[3]); \
	break; \
case EOperationType::I3x8I: \
	gen.BLCST(Operands[2].SingleVector, Operands[2].SingleVector); \
	gen.CALC(Operands[0].MultiVector[0], Operands[1].MultiVector[0], Operands[2].SingleVector); \
	gen.CALC(Operands[0].MultiVector[1], Operands[1].MultiVector[1], Operands[2].SingleVector); \
	gen.CALC(Operands[0].MultiVector[2], Operands[1].MultiVector[2], Operands[2].SingleVector); \
	gen.CALC(Operands[0].MultiVector[3], Operands[1].MultiVector[3], Operands[2].SingleVector); \
	break; \
case EOperationType::I3x8I3: \
	gen.MOVH(Operands[2].SingleVector, Operands[2].SingleVector); \
	gen.CALC(Operands[0].MultiVector[0], Operands[1].MultiVector[0], Operands[2].SingleVector); \
	gen.CALC(Operands[0].MultiVector[1], Operands[1].MultiVector[1], Operands[2].SingleVector); \
	gen.CALC(Operands[0].MultiVector[2], Operands[1].MultiVector[2], Operands[2].SingleVector); \
	gen.CALC(Operands[0].MultiVector[3], Operands[1].MultiVector[3], Operands[2].SingleVector); \
	break; \
case EOperationType::I3x8I3x4: \
	gen.CALC(Operands[0].MultiVector[0], Operands[1].MultiVector[0], Operands[2].MultiVector[0]); \
	gen.CALC(Operands[0].MultiVector[1], Operands[1].MultiVector[1], Operands[2].MultiVector[1]); \
	gen.CALC(Operands[0].MultiVector[2], Operands[1].MultiVector[2], Operands[2].MultiVector[0]); \
	gen.CALC(Operands[0].MultiVector[3], Operands[1].MultiVector[3], Operands[2].MultiVector[1]); \
	break; \

#define VECTOR_MNS_CALC(CALC, BLCST, MOVH) \
case EOperationType::II: \
	gen.CALC(Operands[0].SingleVector, Operands[1].SingleVector, MinusVector); \
	break; \
case EOperationType::I2I2: \
	/* 0bit              256bit */ \
	/* x y na na na na na na    */ \
	gen.CALC(Operands[0].SingleVector, Operands[1].SingleVector, MinusVector); \
	break; \
case EOperationType::I3I3: \
	/* x y z na na na na na */ \
	gen.CALC(Operands[0].SingleVector, Operands[1].SingleVector, MinusVector); \
	break; \
case EOperationType::I3x4I3x4: \
	/* ymm0 ax ay az na bx by bz na */ \
	/* ymm1 cx cy cz na dx dy dz na */ \
	gen.CALC(Operands[0].MultiVector[0], Operands[1].MultiVector[0], MinusVector); \
	gen.CALC(Operands[0].MultiVector[1], Operands[1].MultiVector[1], MinusVector); \
	break; \
case EOperationType::I3x8I3x8: \
	/* ymm0 ax ay az na bx by bz na */ \
	/* ymm1 cx cy cz na dx dy dz na */ \
	/* ymm2 ex ey ez na fx fy fz na */ \
	/* ymm3 gx gy gz na hx hy hz na */ \
	gen.CALC(Operands[0].MultiVector[0], Operands[1].MultiVector[0], MinusVector); \
	gen.CALC(Operands[0].MultiVector[1], Operands[1].MultiVector[1], MinusVector); \
	gen.CALC(Operands[0].MultiVector[2], Operands[1].MultiVector[2], MinusVector); \
	gen.CALC(Operands[0].MultiVector[3], Operands[1].MultiVector[3], MinusVector); \
	break; \

#define GP2_CALC(GPCALC, VICALC, VIBLCST, VFCALC, VFBLCST, VMOVH) \
if (Ins.Type == EOperationType::II) \
{ \
	auto Operands = Table.Get2Operands(DestType, Ins.Operand0, Ins.Operand1, Ins.Operand2, i); \
	gen.GPCALC(Operands[0].SingleInt, Operands[1].SingleInt); \
} \
else \
{ \
	auto Operands = Table.Get3Operands(DestType, Ins.Operand0, Ins.Operand1, Ins.Operand2, i); \
	switch (Ins.Type) \
	{ \
	VECTOR_CALC(VICALC, VIBLCST, VMOVH); \
	default: \
		switch ((EOperationType)((uint)Ins.Type - (uint)EOperationType::RR)) \
		{ \
		VECTOR_CALC(VFCALC, VFBLCST, VMOVH); \
		default: \
			assert(false); \
		} \
	} \
} \

#define GP3_CALC(GPCALC, VICALC, VIBLCST, VFCALC, VFBLCST, VMOVH) \
auto Operands = Table.Get3Operands(DestType, Ins.Operand0, Ins.Operand1, Ins.Operand2, i); \
switch (Ins.Type) \
{ \
case EOperationType::II: \
	gen.GPCALC(Operands[0].SingleInt, Operands[1].SingleInt, Operands[2].SingleInt); \
	break; \
VECTOR_CALC(VICALC, VIBLCST, VMOVH); \
default: \
	switch ((EOperationType)((uint)Ins.Type - (uint)EOperationType::RR)) \
	{ \
	VECTOR_CALC(VFCALC, VFBLCST, VMOVH); \
	default: \
		assert(false); \
	} \
} \

void FCompiler::Run(const vector<SInstruction>& Instructions)
{
	Xbyak::CodeGenerator gen(50000);
	FRegisterTable Table;

	for (uint i = 0; i < Instructions.size(); i++)
	{
		auto& Ins = Instructions[i];
		auto RegisterType = GetDestRegisterType(Ins.Type);
		switch (Ins.Code)
		{
		case EInstruction::ADD:
		case EInstruction::SUB:
		case EInstruction::MUL:
		case EInstruction::DIV:
		case EInstruction::MNS:
			Table.PrimaryRead(Ins.Operand1, i);
			Table.PrimaryRead(Ins.Operand2, i);
			break;
		case EInstruction::STOREA:
		case EInstruction::LOADA:
			Table.PrimarySetUsage(Ins.Operand0, EVirtualRegisterUsage::Argument, RegisterType);
			break;
		case EInstruction::STORER:
		case EInstruction::LOADR:
			Table.PrimarySetUsage(Ins.Operand0, EVirtualRegisterUsage::Return, RegisterType);
			break;
		default:
			break;
		}
	}

	for (uint i = 0; i < Instructions.size(); i++)
	{
		auto& Ins = Instructions[i];
		auto DestType = GetDestRegisterType(Ins.Type);
		EInstruction e;
		switch (e)
		{
		case EInstruction::LABEL:
			break;
		case EInstruction::ADD:GP2_CALC(add, vpaddd, vpbroadcastd, vaddps, vbroadcastss, movlhps) break;
		case EInstruction::SUB:GP2_CALC(sub, vpsubd, vpbroadcastd, vsubps, vbroadcastss, movlhps) break;
		case EInstruction::MUL:GP2_CALC(imul, vpmulld, vpbroadcastd, vmulps, vbroadcastss, movlhps) break;
		case EInstruction::DIV:GP2_CALC(add, vpaddd, vpbroadcastd, vdivps, vbroadcastss, movlhps) break;
		case EInstruction::MOD:break;
		case EInstruction::MNS:
			if (Ins.Type == EOperationType::II)
			{
				auto Operand = Table.Use(Ins.Operand0, i);
				gen.neg(Operand.SingleInt);
			}
			else
			{
				auto Operands = Table.Get3Operands(DestType, Ins.Operand0, Ins.Operand1, Ins.Operand2, i);
				auto MinusVector = Table.GetTemporarySIMDRegister();
				//gen.vbroadcastss(MinusVector, 1);
				switch (Ins.Type)
				{
					VECTOR_MNS_CALC(vpmulld, vpbroadcastd, movlhps);
				default:
					switch ((EOperationType)((uint)Ins.Type - (uint)EOperationType::RR))
					{
						VECTOR_MNS_CALC(vmulps, vbroadcastss, movlhps);
					default:
						assert(false);
					}
				}
			}
			break;
		case EInstruction::ICE:
			break;
		case EInstruction::ICNE:
			break;
		case EInstruction::ICG:
			break;
		case EInstruction::ICL:
			break;
		case EInstruction::ICGE:
			break;
		case EInstruction::ICLE:
			break;
		case EInstruction::FCE:
			break;
		case EInstruction::FCNE:
			break;
		case EInstruction::FCG:
			break;
		case EInstruction::FCL:
			break;
		case EInstruction::FCGE:
			break;
		case EInstruction::FCLE:
			break;
		case EInstruction::LN:
			break;
		case EInstruction::LAND:
			break;
		case EInstruction::LOR:
			break;
		case EInstruction::SIN:
			break;
		case EInstruction::COS:
			break;
		case EInstruction::TAN:
			break;
		case EInstruction::INSERT:
			break;
		case EInstruction::VNORM:
			break;
		case EInstruction::VCROS:
			break;
		case EInstruction::VDOT:
			break;
		case EInstruction::EXTRACT:
			break;
		case EInstruction::LOAD:
			break;
		case EInstruction::LOADA:
			break;
		case EInstruction::LOADR:
			break;
		case EInstruction::STOREA:
			break;
		case EInstruction::STORER:
			break;
		case EInstruction::JMP:
			break;
		case EInstruction::IFJMP:
			break;
		case EInstruction::IFNJMP:
			break;
		case EInstruction::MOVE:
			break;
		case EInstruction::CALL:
			break;
		case EInstruction::ENTRY:
			break;
		case EInstruction::RETURN:
			break;
		case EInstruction::END:
			break;
		case EInstruction::PUSHV:
			break;
		case EInstruction::PUSHI:
			break;
		case EInstruction::PUSHS:
			break;
		case EInstruction::PUSHSI:
			break;
		case EInstruction::ERROR:
			break;
		default:
			break;
		}
	}
}
