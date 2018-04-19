#include "Expression.h"
#include "Utility.h"

SOperation GetOperation(const SToken& Operator, SType LeftType, SType RightType)
{	
	if (LeftType.ValueType != RightType.ValueType)
		return EInstruction::ERROR;

	auto OperatorUint = ToUint64(Operator.String);
	auto IsSingle = LeftType.Count == 1;

	if (IsSingle && LeftType.ValueType == EValueType::Bool)
	{
		switch (OperatorUint)
		{
		case "&&"u64: 
			return EInstruction::AND;
		case "||"u64: 
			return EInstruction::OR;
		default: 
			return EInstruction::ERROR;
		}
	}

	auto IsEqual = LeftType.Equals(RightType);
	auto IsBloadcast = LeftType.CanBloadcast(RightType);

	if (IsEqual || IsBloadcast)
	{
		switch (OperatorUint)
		{
		case '+': 
			return SOperation(EInstruction::ADD, IsBloadcast);
		case '-': 
			return SOperation(EInstruction::SUB, IsBloadcast);
		case '*': 
			return SOperation(EInstruction::MUL, IsBloadcast);
		case '/': 
			if(IsSingle && LeftType.ValueType == EValueType::Int)
				return SOperation(EInstruction::DIV, IsBloadcast);
			else 
				return EInstruction::ERROR;
		case '%': 
			if (IsSingle && LeftType.ValueType == EValueType::Int)
				return SOperation(EInstruction::MOD, IsBloadcast);
			else
				return EInstruction::ERROR;
		case "=="u64: 
			if (IsSingle)
				return SOperation(EInstruction::CMPE, IsBloadcast);
			else
				return EInstruction::ERROR;
		case "!="u64: 
			if (IsSingle)
				return SOperation(EInstruction::CMPE, IsBloadcast);
			else
				return EInstruction::ERROR;
		case '<': 
			if (IsSingle)
				return SOperation(EInstruction::CMPG, IsBloadcast);
			else
				return EInstruction::ERROR;
		case '>': 
			if (IsSingle)
				return SOperation(EInstruction::CMPL, IsBloadcast);
			else
				return EInstruction::ERROR;
		case "<="u64: 
			if (IsSingle)
				return SOperation(EInstruction::CMPGE, IsBloadcast);
			else
				return EInstruction::ERROR;
		case ">="u64: 
			if (IsSingle)
				return SOperation(EInstruction::CMPLE, IsBloadcast);
			else
				return EInstruction::ERROR;
		default: 
			return EInstruction::ERROR;
		}
	}

	return EInstruction::ERROR;
}

SOperation GetOperation(const SToken& Operator, const SType& Type)
{
	switch (Operator.String[0])
	{
	case '-': return EInstruction::MNS;
	case '!': if (Type.ValueType == EValueType::Bool)return EInstruction::NOT;
	}

	return EInstruction::ERROR;
}

void GenerateLoadCode(const SCompilingData& Data, const SType& Type, const SLiteral& Literal, regptr Register)
{
	auto DataPointer = Data.Program->GetDataIndex();
	auto Size = Type.GetSize();

	for (uint i = 0; i < Size; i++)
		Data.Program->AddData(*((uint8*)Literal.GetPointer() + i));

	Data.Routine->AddInstruction(EInstruction::LOAD, Type, Register, DataPointer, 0);
}

regptr FExpression::GenerateCode(const SCompilingData& Data, regptr Register)
{
	auto Result = GenerateCodeOrLiteral(Data, Register);
	if (Result.IsLiteralVaild)
	{
		GenerateLoadCode(Data, Type, Result.Literal, Register);
		return Register;
	}
	else
	{
		return Result.Register;
	}
}

void FExpression::GenerateCodeOrMove(const SCompilingData& Data, regptr Register)
{
	auto ResultRegister = GenerateCode(Data, Register);

	if (Register != ResultRegister)
	{
		Data.Routine->AddInstruction(EInstruction::MOVE, Type, Register, ResultRegister, 0);
	}
}

SExpressionGenerationResult FExpressionLiteral::GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register)
{
	switch (Type.ValueType)
	{
	case EValueType::Real: 
		return SLiteral(stof(Value.String));
	case EValueType::Int: 
		return SLiteral((long)stoi(Value.String));
	case EValueType::Bool: 
		return SLiteral((long)strcmp(Value.String, "false"));
	default:
		Data.Program->AddError(Value.Location, "不明な型です。");
		return nullptr;
	}
}

SExpressionGenerationResult FExpressionVariable::GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register)
{
	auto Variable = Data.Scope->FindVariable(Name.String);
	if (!Variable)
	{
		Data.Program->AddError(Name.Location, string("変数'") + Name.String + "'は宣言されていません。");
		return nullptr;
	}

	Type = Variable->Type;
	if (Variable->IsLiteralVaild)
		return Variable->Literal;
	else
		return Variable->Register;
}

SExpressionGenerationResult FExpressionOperation::GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register)
{
	auto StackPointer = Data.Scope->GetStackPointer();

	auto LeftRequestRegister = StackPointer;
	auto LeftResult = Left->GenerateCodeOrLiteral(Data, LeftRequestRegister);
	auto RegisterOffset = LeftResult.Register == LeftRequestRegister ? Left->GetType().GetSize() : 0;

	Data.Scope->ReserveRegister(RegisterOffset);
	auto RightRequestRegister = StackPointer + RegisterOffset;
	auto RightResult = Right->GenerateCodeOrLiteral(Data, RightRequestRegister);
	Data.Scope->ReleaseRegister(RegisterOffset);

	auto Operation = GetOperation(Operator, Left->GetType(), Right->GetType());

	Type = Left->GetType();

	if (LeftResult.IsLiteralVaild && RightResult.IsLiteralVaild)
	{
		auto Literal = SLiteral::Empty(Type.GetSize());
		Type.Calculate(Operation, Literal.GetPointer(), LeftResult.Literal.GetPointer(), RightResult.Literal.GetPointer());
		return Literal;
	}
	else
	{
		auto LeftActualRegister = LeftResult.Register;
		auto RightActualRegister = RightResult.Register;

		if (LeftResult.IsLiteralVaild)
		{
			GenerateLoadCode(Data, Left->GetType(), LeftResult.Literal, LeftRequestRegister);
			LeftActualRegister = LeftRequestRegister;
		}

		if (RightResult.IsLiteralVaild)
		{
			GenerateLoadCode(Data, Right->GetType(), RightResult.Literal, RightRequestRegister);
			RightActualRegister = RightRequestRegister;
		}

		if (Operation.Code == EInstruction::ERROR)
		{
			auto Error = "型'" + Left->GetType().ToString() + "'" + "と型'" + Right->GetType().ToString() + "'で演算子'" + Operator.String + "'は使用できません。";
			Data.Program->AddError(Operator.Location, Error);
			return nullptr;
		}	

		Data.Routine->AddInstruction(Operation, Type, Register, LeftActualRegister, RightActualRegister);
		return Register;
	}
}

SExpressionGenerationResult FExpressionUnaryOperation::GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register)
{
	Type = Unary->GetType();
	auto Result = Unary->GenerateCodeOrLiteral(Data, Register);
	auto Operation = GetOperation(Operator, Unary->GetType());

	if (Result.IsLiteralVaild)
	{
		auto Literal = SLiteral::Empty(Type.GetSize());
		Type.Calculate(Operation, Literal.GetPointer(), Result.Literal.GetPointer());
		return Literal;
	}
	else
	{
		Data.Routine->AddInstruction(Operation, Type, Register, Result.Register, 0);
		return Register;
	}
}

SExpressionGenerationResult FExpressionFunctionCall::GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register)
{
	auto Function = Data.Scope->FindFunction(Name.String);
	if (!Function)
	{
		Data.Program->AddError(Name.Location, string("関数'") + Name.String + "'は定義されていません。");
		return nullptr;
	}
	
	Type = Function->ReturnType;

	if (Function->IsBuiltInNotDefine)
	{
		auto ArgumentRegisters = GenerateCodeArguments(Data, Arguments);
		return Function->GenerateCode(Data, ArgumentRegisters);
	}
	else
	{
		auto StackPointer = Data.Scope->GetStackPointer();
		GenerateCodeStoreArguments(Data, Arguments, StackPointer);
		Data.Routine->AddInstruction(EInstruction::CALL, Function->Label, StackPointer, 0);
		return StackPointer;
	}	
	
	if (!CheckArguments("関数", Name.String, Data, Function->ArgumentTypes, Arguments))
		return nullptr;
}

SExpressionGenerationResult FExpressionAccessor::GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register)
{
	Type = Target->GetType();
	auto Result = Target->GenerateCodeOrLiteral(Data, Register);

	uint Index = Member.String[0] - 'a';
	if (Type.Count < Index)Index = Member.String[0] - 'x';
	if (Index < 0 || Type.Count <= Index)
	{
		Data.Program->AddError(Member.Location, "型'" + Type.ToString() + "'にメンバー'" + Member.String + "'はありません。");
		return nullptr;
	}

	if (Result.IsLiteralVaild)
	{
		auto Literal = SLiteral::Empty(Type.Count);
		Type.Extract(Literal.GetPointer(), Result.Literal.GetPointer(), Index);
		return Literal;
	}
	else
	{
		auto ExpressionRegister = Target->GenerateCode(Data, Register);
		Data.Routine->AddInstruction(EInstruction::EXTRACT, Type, Register, ExpressionRegister, Index);
		return Register;
	}
}

SExpressionGenerationResult FExpressionConvert::GenerateCodeOrLiteral(const SCompilingData& Data, regptr Register)
{
	auto Result = Target->GenerateCodeOrLiteral(Data, Register);

	Type = Target->GetType();
	Type.ValueType = DestValueType;

	if (Result.IsLiteralVaild)
	{
		auto Literal = SLiteral::Empty(Type.GetSize());
		Target->GetType().Convert(DestValueType, Literal.GetPointer(), Result.Literal.GetPointer());
		return Literal;
	}
	else
	{
		Data.Routine->AddInstruction(EInstruction::CAST, Type, Register, Result.Register, 0);
		return Register;
	}
}