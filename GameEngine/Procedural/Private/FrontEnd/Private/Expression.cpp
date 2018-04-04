#include "Expression.h"
#include "Utility.h"

EInstruction GetInstructionCode(const SToken& Operator, EValueType LeftType, EValueType RightType)
{
	auto OperatorUint = ToUint64(Operator.String);

	if (LeftType == EValueType::Boolean && RightType == EValueType::Boolean)
	{
		switch (OperatorUint)
		{
		case "&&"u64: return EInstruction::LAND;
		case "||"u64: return EInstruction::LOR;
		}
	}
	switch (OperatorUint)
	{
	case '+': return EInstruction::ADD;
	case '-': return EInstruction::SUB;
	case '*': return EInstruction::MUL;
	case '/': return EInstruction::DIV;
	case '%': return EInstruction::MOD;
	case "=="u64: return EInstruction::FCE;
	case "!="u64: return EInstruction::FCE;
	case '<': return EInstruction::FCG;
	case '>': return EInstruction::FCL;
	case "<="u64: return EInstruction::FCGE;
	case ">="u64: return EInstruction::FCLE;
	}
	return EInstruction::ERROR;
}

EInstruction GetInstructionCode(const SToken& Operator, EValueType Type)
{
	switch (Operator.String[0])
	{
	case '-':
		return EInstruction::MNS;
	case '!':
		if (Type == EValueType::Boolean)return EInstruction::LN;
	}
	return EInstruction::ERROR;
}

void GenerateLoadCode(const SCompilingData& Data, EValueType Type, const SLiteral& Literal, regptr Register)
{
	auto Size = GetTypeSize(Type);
	auto Index = Data.Intermediate->GetDataIndex();
	for (uint i = 0; i < Size / 4; i++)Data.Intermediate->AddData(*(&Literal.Int + i));
	auto OperationType = GetOperationType(Type);
	Data.Routine->AddInstruction(EInstruction::LOAD, OperationType, Register, Index, 0);
}

regptr FExpression::OnlyGenerateCode(const SCompilingData& Data, regptr Register)
{
	auto Literal = OnlyGetLiteral();
	if (Literal.IsValid)
	{
		GenerateLoadCode(Data, Type, Literal, Register);
		return Register;
	}
	else return OnlyGenerateCodeWithoutLiteral(Data, Register);
}

regptr FExpression::GenerateCode(const SCompilingData& Data, regptr Register)
{
	if (IsResolvedSymbols)
	{
		ResolveSymbols(Data);
		IsResolvedSymbols = true;
	}
	return OnlyGenerateCode(Data, Register);
}

void FExpression::GenerateCodeOrMove(const SCompilingData& Data, regptr Register)
{
	auto ResultRegister = GenerateCode(Data, Register);
	if (Register != ResultRegister)
	{
		auto OperationType = GetOperationType(Type);
		Data.Routine->AddInstruction(EInstruction::MOVE, OperationType, Register, ResultRegister, 0);
	}
}

SLiteral FExpression::GetLiteral(const SCompilingData& Data)
{
	if (IsResolvedSymbols)
	{
		ResolveSymbols(Data);
		IsResolvedSymbols = true;
	}
	return OnlyGetLiteral();
}

void FExpressionLiteral::ResolveSymbols(const SCompilingData& Data)
{	
	auto Literal = OnlyGetLiteral();
	if(!Literal.IsValid)Data.Intermediate->AddError(Value.Location, "不明な型です。");
}

regptr FExpressionLiteral::OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const
{
	GenerateLoadCode(Data, Type, Literal, Register);
	return Register;
}

SLiteral FExpressionLiteral::OnlyGetLiteral() const
{
	switch (Type)
	{
	case EValueType::Real: return SLiteral(stof(Value.String));
	case EValueType::Int: return SLiteral((long)stoi(Value.String));
	case EValueType::Boolean: return SLiteral((long)strcmp(Value.String, "false"));
	}
	return SLiteral();
}

void FExpressionVariable::ResolveSymbols(const SCompilingData& Data)
{
	Variable = Data.Scope->FindVariable(Name.String);
	if(Variable)Type = Variable->Type;
	else
	{
		Type = EValueType::Error;
		Data.Intermediate->AddError(Name.Location, string("'") + Name.String + "'は宣言されていない変数です。");
	}
}

regptr FExpressionVariable::OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const
{
	return Variable->Register;
}

SLiteral FExpressionVariable::OnlyGetLiteral() const
{
	return Variable->Literal;
}

void FExpressionOperation::ResolveSymbols(const SCompilingData& Data)
{
	Left->ResolveSymbols(Data);
	Right->ResolveSymbols(Data);

	OperationCode = GetInstructionCode(Operator, Left->GetType(), Right->GetType());
	OperationType = GetOperationType(Left->GetType(), Right->GetType());

	if (OperationCode == EInstruction::ERROR || OperationType == EOperationType::Error)
	{
		auto LeftTypeName = GetTypeName(Left->GetType());
		auto RightTypeName = GetTypeName(Right->GetType());
		auto Error = "型'" + LeftTypeName + "'" + "と型'" + RightTypeName + "'で演算子'" + Operator.String + "'は使用できません。";
		Data.Intermediate->AddError(Operator.Location, Error);
	}
	else if (OperationCode == EInstruction::DIV && OperationType > EOperationType::II && OperationType < EOperationType::RR)
	{
		Data.Intermediate->AddError(Operator.Location, "整数ベクトル演算で除算は使用できません。");
	}
	else if (OperationCode == EInstruction::MOD && OperationType != EOperationType::II)
	{
		Data.Intermediate->AddError(Operator.Location, "整数スカラー演算以外で剰余演算は使用できません。");
	}

	Type = Left->GetType();
}

regptr FExpressionOperation::OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const
{
	auto StackPointer = Data.Scope->GetStackPointer();
	auto LeftRegister = Left->GenerateCode(Data, StackPointer);
	auto RegisterOffset = LeftRegister == StackPointer ? GetTypeSize(Left->GetType()) : 0;
	Data.Scope->ReserveRegister(RegisterOffset);
	auto RightRegister = Right->GenerateCode(Data, StackPointer + RegisterOffset);
	Data.Scope->ReleaseRegister(RegisterOffset);
	auto OperationType = GetOperationType(Left->GetType(), Right->GetType());
	Data.Routine->AddInstruction(OperationCode, OperationType, Register, LeftRegister, RightRegister);
	return Register;
}

SLiteral FExpressionOperation::OnlyGetLiteral() const
{
	auto LeftLiteral = Left->OnlyGetLiteral();
	auto RightLiteral = Right->OnlyGetLiteral();
	if (!LeftLiteral.IsValid || !RightLiteral.IsValid)return SLiteral();

	EOperationType OperationType;
	switch (OperationCode)
	{
	case EInstruction::ADD:
		switch (OperationType)
		{
		case EOperationType::II:return SLiteral(LeftLiteral.Int + RightLiteral.Int);
		case EOperationType::I2I2:return SLiteral(LeftLiteral.Int2 + RightLiteral.Int2);
		case EOperationType::I2I:return SLiteral(LeftLiteral.Int2 + RightLiteral.Int);
		case EOperationType::I3I:return SLiteral(LeftLiteral.Int3 + RightLiteral.Int);
		case EOperationType::I3I3:return SLiteral(LeftLiteral.Int3 + RightLiteral.Int3);
		case EOperationType::RR:return SLiteral(LeftLiteral.Real + RightLiteral.Real);
		case EOperationType::R2R2:return SLiteral(LeftLiteral.Real2 + RightLiteral.Real2);
		case EOperationType::R2R:return SLiteral(LeftLiteral.Real2 + RightLiteral.Real);
		case EOperationType::R3R:return SLiteral(LeftLiteral.Real3 + RightLiteral.Real);
		case EOperationType::R3R3:return SLiteral(LeftLiteral.Real3 + RightLiteral.Real3);
		}
		break;
	case EInstruction::SUB:
		switch (OperationType)
		{
		case EOperationType::II:return SLiteral(LeftLiteral.Int - RightLiteral.Int);
		case EOperationType::I2I2:return SLiteral(LeftLiteral.Int2 - RightLiteral.Int2);
		case EOperationType::I2I:return SLiteral(LeftLiteral.Int2 - RightLiteral.Int);
		case EOperationType::I3I:return SLiteral(LeftLiteral.Int3 - RightLiteral.Int);
		case EOperationType::I3I3:return SLiteral(LeftLiteral.Int3 - RightLiteral.Int3);
		case EOperationType::RR:return SLiteral(LeftLiteral.Real - RightLiteral.Real);
		case EOperationType::R2R2:return SLiteral(LeftLiteral.Real2 - RightLiteral.Real2);
		case EOperationType::R2R:return SLiteral(LeftLiteral.Real2 - RightLiteral.Real);
		case EOperationType::R3R:return SLiteral(LeftLiteral.Real3 - RightLiteral.Real);
		case EOperationType::R3R3:return SLiteral(LeftLiteral.Real3 - RightLiteral.Real3);
		}
		break;
	case EInstruction::MUL:
		switch (OperationType)
		{
		case EOperationType::II:return SLiteral(LeftLiteral.Int * RightLiteral.Int);
		case EOperationType::I2I2:return SLiteral(LeftLiteral.Int2 * RightLiteral.Int2);
		case EOperationType::I2I:return SLiteral(LeftLiteral.Int2 * RightLiteral.Int);
		case EOperationType::I3I:return SLiteral(LeftLiteral.Int3 * RightLiteral.Int);
		case EOperationType::I3I3:return SLiteral(LeftLiteral.Int3 * RightLiteral.Int3);
		case EOperationType::RR:return SLiteral(LeftLiteral.Real * RightLiteral.Real);
		case EOperationType::R2R2:return SLiteral(LeftLiteral.Real2 * RightLiteral.Real2);
		case EOperationType::R2R:return SLiteral(LeftLiteral.Real2 * RightLiteral.Real);
		case EOperationType::R3R:return SLiteral(LeftLiteral.Real3 * RightLiteral.Real);
		case EOperationType::R3R3:return SLiteral(LeftLiteral.Real3 * RightLiteral.Real3);
		}
		break;
	case EInstruction::DIV:
		switch (OperationType)
		{
		case EOperationType::II:return SLiteral(LeftLiteral.Int / RightLiteral.Int);
		case EOperationType::I2I2:return SLiteral(LeftLiteral.Int2 / RightLiteral.Int2);
		case EOperationType::I2I:return SLiteral(LeftLiteral.Int2 / RightLiteral.Int);
		case EOperationType::I3I:return SLiteral(LeftLiteral.Int3 / RightLiteral.Int);
		case EOperationType::I3I3:return SLiteral(LeftLiteral.Int3 / RightLiteral.Int3);
		case EOperationType::RR:return SLiteral(LeftLiteral.Real / RightLiteral.Real);
		case EOperationType::R2R2:return SLiteral(LeftLiteral.Real2 / RightLiteral.Real2);
		case EOperationType::R2R:return SLiteral(LeftLiteral.Real2 / RightLiteral.Real);
		case EOperationType::R3R:return SLiteral(LeftLiteral.Real3 / RightLiteral.Real);
		case EOperationType::R3R3:return SLiteral(LeftLiteral.Real3 / RightLiteral.Real3);
		}
		break;
	}
	return SLiteral();
}

void FExpressionUnaryOperation::ResolveSymbols(const SCompilingData& Data)
{
	Unary->ResolveSymbols(Data);
	if (Unary)
	{
		OperationCode = GetInstructionCode(Operator, Unary->GetType());
		OperationType = GetOperationType(Unary->GetType());
		Type = Unary->GetType();
	}
	else Type = EValueType::Error;
}

regptr FExpressionUnaryOperation::OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const
{
	auto UnaryRegister = Unary->GenerateCode(Data, Register);
	Data.Routine->AddInstruction(OperationCode, EOperationType::None, Register, UnaryRegister, 0);
	return Register;
}

SLiteral FExpressionUnaryOperation::OnlyGetLiteral() const
{
	auto UnaryLiteral = Unary->OnlyGetLiteral();

	switch (OperationCode)
	{
	case EInstruction::MNS:
		switch (OperationType)
		{
		case EOperationType::II:return SLiteral(-UnaryLiteral.Int);
		case EOperationType::I2I2:return SLiteral(-UnaryLiteral.Int2);
		case EOperationType::I3I3:return SLiteral(-UnaryLiteral.Int3);
		case EOperationType::RR:return SLiteral(-UnaryLiteral.Real);
		case EOperationType::R2R2:return SLiteral(-UnaryLiteral.Real2);
		case EOperationType::R3R3:return SLiteral(-UnaryLiteral.Real3);
		}
		break;
	case EInstruction::LN:
		switch (OperationType)
		{
		case EOperationType::BB:return SLiteral(!UnaryLiteral.Bool);
		}
		break;
	}
	return SLiteral();
}

void FExpressionFunctionCall::ResolveSymbols(const SCompilingData& Data)
{
	Function = Data.Scope->FindFunction(Name.String);
	if(Function)Type = Function->ReturnType;
	else Data.Intermediate->AddError(Name.Location, string("'") + Name.String + "'は宣言されていない関数です。");

	uint SpecifiedSize = Arguments.size();
	uint DefinedSize = Function->ArgumentTypes.size();
	if (SpecifiedSize != DefinedSize)
	{
		auto Location = SLocation::Between((*Arguments.begin())->GetLocation(), (*(Arguments.end() - 1))->GetLocation());
		auto Message = string("関数'") + Name.String + "'に" + to_string(SpecifiedSize) + "個の引数を指定できません。" + to_string(DefinedSize) + "個の引数を指定してください。";
		Data.Intermediate->AddError(Location, Message);
	}

	for (uint i = 0; i < Arguments.size(); i++)
	{
		auto SpecifiedType = Arguments[i]->GetType();
		auto DefinedType = Function->ArgumentTypes[i];

		if (SpecifiedType != DefinedType)
		{
			auto SpecifiedTypeName = GetTypeName(SpecifiedType);
			auto DefinedTypeName = GetTypeName(DefinedType);
			auto Message = "型'" + SpecifiedTypeName + "'は関数'" + Name.String + "'の第" + to_string(i + 1) + "引数に指定できません。型'" + DefinedTypeName + "'を指定してください。";
			Data.Intermediate->AddError(Arguments[i]->GetLocation(), Message);
		}
	}
}

regptr FExpressionFunctionCall::OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const
{
	auto StackPointer = Data.Scope->GetStackPointer();
	GenerateCodeStoreArguments(Data, Arguments, StackPointer, 0);
	Data.Routine->AddInstruction(EInstruction::CALL, EOperationType::None, Function->Label, StackPointer, 0);
	return StackPointer;
}

SLiteral FExpressionFunctionCall::OnlyGetLiteral() const
{
	return SLiteral();
}

void FExpressionAccessor::ResolveSymbols(const SCompilingData& Data)
{
	Expression->ResolveSymbols(Data);
	Type = Expression->GetType();

	char MemberChar = Member.String[0];
	MemberIndex = -1;
	switch (Type)
	{
	case EValueType::Int2:
		MemberIndex = MemberChar - 'x';
		if (MemberIndex >= 2)MemberIndex = -1;
		OperationType = EOperationType::I2I;
		break;
	case EValueType::Int3:
		MemberIndex = MemberChar - 'x';
		if (MemberIndex >= 3)MemberIndex = -1;
		OperationType = EOperationType::I3I;
		break;
	case EValueType::Int3x4:
		MemberIndex = MemberChar - 'a';
		if (MemberIndex >= 4)MemberIndex = -1;
		OperationType = EOperationType::I3x4I3;
		break;
	case EValueType::Int3x8:
		MemberIndex = MemberChar - 'a';
		if (MemberIndex >= 8)MemberIndex = -1;
		OperationType = EOperationType::I3x8I3;
		break;
	case EValueType::Real2:
		MemberIndex = MemberChar - 'x';
		if (MemberIndex >= 2)MemberIndex = -1;
		OperationType = EOperationType::R2R;
		break;
	case EValueType::Real3:
		MemberIndex = MemberChar - 'x';
		if (MemberIndex >= 3)MemberIndex = -1;
		OperationType = EOperationType::R3R;
		break;
	case EValueType::Real3x4:
		MemberIndex = MemberChar - 'a';
		if (MemberIndex >= 4)MemberIndex = -1;
		OperationType = EOperationType::R3x4R3;
		break;
	case EValueType::Real3x8:
		MemberIndex = MemberChar - 'a';
		if (MemberIndex >= 8)MemberIndex = -1;
		OperationType = EOperationType::R3x8R3;
		break;
	}

	if (MemberIndex < 0)
	{
		auto TypeName = GetTypeName(Expression->GetType());
		Data.Intermediate->AddError(Member.Location, "型'" + TypeName + "'にメンバー'" + Member.String + "'はありません。");
	}
}

regptr FExpressionAccessor::OnlyGenerateCodeWithoutLiteral(const SCompilingData& Data, regptr Register) const
{
	auto ExpressionRegister = Expression->GenerateCode(Data, Register);
	Data.Routine->AddInstruction(EInstruction::EXTRACT, EOperationType::I2I, Register, ExpressionRegister, MemberIndex);
	return Register;
}

SLiteral FExpressionAccessor::OnlyGetLiteral() const
{
	auto Literal = Expression->OnlyGetLiteral();
	if (!Literal.IsValid)return SLiteral();

	switch (Type)
	{
	case EValueType::Int2:
		switch (Member.String[0])
		{
		case 'x':return SLiteral(Literal.Int2.X);
		case 'y':return SLiteral(Literal.Int2.Y);
		}
		break;
	case EValueType::Int3:
		switch (Member.String[0])
		{
		case 'x':return SLiteral(Literal.Int3.X);
		case 'y':return SLiteral(Literal.Int3.Y);
		case 'z':return SLiteral(Literal.Int3.Z);
		}
		break;
	case EValueType::Real2:
		switch (Member.String[0])
		{
		case 'x':return SLiteral(Literal.Real2.X);
		case 'y':return SLiteral(Literal.Real2.Y);
		}
		break;
	case EValueType::Real3:
		switch (Member.String[0])
		{
		case 'x':return SLiteral(Literal.Real3.X);
		case 'y':return SLiteral(Literal.Real3.Y);
		case 'z':return SLiteral(Literal.Real3.Z);
		}
		break;
	}
	return SLiteral();
}