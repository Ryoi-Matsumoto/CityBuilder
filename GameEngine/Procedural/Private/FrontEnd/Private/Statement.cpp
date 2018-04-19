#include "Statement.h"
#include "Expression.h"
#include "Utility.h"

struct SCompilingSubscopeEntity
{
	SCompilingData Data;
	FScope Subscope;

	SCompilingSubscopeEntity(const SCompilingData& InData, regptr RegisterOffset = 0) 
		: Data(InData), Subscope(InData.Scope, RegisterOffset)
	{
		Data.Scope = &Subscope;
	}
};

struct SCompilingSubroutineEntity : public SCompilingSubscopeEntity
{
	FRoutine* Mainroutine;
	FRoutine Subroutine;

	SCompilingSubroutineEntity(const SCompilingData& InData, SProcedure* CurrentProcedure) 
		: SCompilingSubscopeEntity(InData), Subroutine(CurrentProcedure)
	{
		Mainroutine = Data.Routine;
		Data.Routine = &Subroutine;
	}

	void Store()
	{
		Mainroutine->StoreRoutine(Subroutine);
	}
};

vector<SType> GetArgumentTypes(vector<FStatementDeclaration*> Arguments)
{
	vector<SType> Result;
	Result.reserve(Arguments.size());
	for (auto Argument : Arguments)
		Result.push_back(Argument->GetType());
	return Result;
}

const SShape* FStatementInvaildToken::GenerateCode(const SCompilingData& Data, const SShape* InputShape)
{
	Data.Program->AddError(Token.Location, string("トークン'") + Token.String + "'は無効です。");
	return InputShape;
}

const SShape* FStatementBlock::GenerateCode(const SCompilingData& Data, const SShape* InputShape)
{
	SCompilingSubscopeEntity Entity(Data);
	auto OutputShape = InputShape;

	for (auto Statement : Statements)
		OutputShape = Statement->GenerateCode(Entity.Data, OutputShape);

	return OutputShape;
}

const SShape* FStatementDefineFunction::GenerateCode(const SCompilingData& Data, const SShape* InputShape)
{
	auto Label = Data.Program->IssueLabel();
	auto CurrentFunction = Data.Scope->AddFunction(Name.String, ReturnType, GetArgumentTypes(Arguments), Block.get(), Label);
	if (!CurrentFunction)
	{
		Data.Program->AddError(Name.Location, string("関数'") + Name.String + "'は既に定義されています。");
		return InputShape;
	}

	SCompilingSubroutineEntity Entity(Data, nullptr);
	{
		Entity.Data.Routine->AddInstruction(EInstruction::LABEL, Label, 0, 0);

		for (auto Argument : Arguments)
			Argument->GenerateCode(Entity.Data, nullptr);
		Block->GenerateCode(Entity.Data, nullptr);

		Entity.Data.Routine->AddInstruction(EInstruction::RETURN, 0, 0, 0);
		Entity.Store();
	}

	return InputShape;
}

const SShape* FStatementDefineProcedure::GenerateCode(const SCompilingData& Data, const SShape* InputShape)
{
	auto Label = Data.Program->IssueLabel();
	auto Shape = Data.Scope->FindShape(TargetShape.String);
	if (!Shape)
	{
		Data.Program->AddError(TargetShape.Location, string("形状'") + TargetShape.String + "'は定義されていません。");
		return InputShape;
	}

	auto CurrentProcedure = Data.Scope->AddProcedure(Name.String, IsTagNotValue, Shape, Block.get(), GetArgumentTypes(Arguments), ForkYieldType, Label);
	if (!CurrentProcedure)
	{
		Data.Program->AddError(Name.Location, string("プロシージャ'") + Name.String + "'は既に定義されています。");
		return InputShape;
	}
	
	SCompilingSubroutineEntity Entity(Data, CurrentProcedure);
	{
		Entity.Data.Routine->AddInstruction(EInstruction::LABEL, Label, 0, 0);

		for (auto Member : Shape->Members)
		{
			FStatementDeclaration ShapeMemberDeclaration(Member.first, Member.second);
			ShapeMemberDeclaration.GenerateCode(Entity.Data, nullptr);
		}

		for (auto Argument : Arguments)
			Argument->GenerateCode(Entity.Data, nullptr);

		Block->GenerateCode(Entity.Data, Data.Scope->FindShape(TargetShape.String));
		Entity.Data.Routine->AddInstruction(EInstruction::RETURN, 0, 0, 0);
		Entity.Store();
	}

	return InputShape;
}

const SShape* FStatementYield::GenerateCode(const SCompilingData& Data, const SShape* InputShape)
{
	auto Procedure = Data.Routine->GetCurrentProcedure();

	if (!Procedure)
	{
		Data.Program->AddError(Value->GetLocation(), "プロシージャ以外ではYieldステートメントは使用できません。");
		return InputShape;
	}

	if (IsTagNotValue)
	{
		auto YieldedShape = Block->GenerateCode(Data, nullptr);
		Data.Routine->AddTagYield(Tag.String, YieldedShape);
		Block->GenerateCode(Data, nullptr);
		
		Procedure->ForkTagYield.push_back(pair<string, const SShape*>(Tag.String, YieldedShape));
	}
	else
	{
		if (Value->GetType().ValueType != EValueType::Int || Value->GetType().Count > 1)
		{
			Data.Program->AddError(Value->GetLocation(), "Yieldステートメントにはタグもしくは整数型以外指定できません。");
			return InputShape;
		}

		auto YieldedShape = Block->GenerateCode(Data, nullptr);
		Procedure->ForkValueYield = YieldedShape;
		Data.Scope->ReserveRegister(YieldedShape->Size);

		auto StackPointer = Data.Scope->GetStackPointer();
		regptr RegisterOffset = 0;

		for (auto Member : YieldedShape->Members)
		{
			auto IndexRegister = Value->GenerateCode(Data, StackPointer + YieldedShape->Size);
			Data.Routine->AddInstruction(EInstruction::STORERA, Member.first, StackPointer, IndexRegister, 0);
		}

		Data.Scope->ReleaseRegister(YieldedShape->Size);
	}

	return InputShape;
}

const SShape* FStatementDefineShape::GenerateCode(const SCompilingData& Data, const SShape* InputShape)
{
	vector<pair<SType, string>> MemberDefines;
	MemberDefines.reserve(Members.size());
	for (auto Member : Members)
	{
		auto MemberName = Member.second.String;
		for (auto MemberDefine : MemberDefines)
		{
			if (MemberName == MemberDefine.second)
			{
				Data.Program->AddError(Member.second.Location, string("メンバー'") + MemberName + "'は既に定義されています。");
				return InputShape;
			}
		}
		MemberDefines.push_back(pair<SType, string>(Member.first, MemberName));
	}

	Data.Scope->AddShape(Name.String, MemberDefines);
	return InputShape;
}

const SShape* FStatementDeclaration::GenerateCode(const SCompilingData& Data, const SShape* InputShape)
{
	auto Register = Data.Scope->GetStackPointer();

	if (Expression)
		Expression->GenerateCode(Data, Register);

	Data.Scope->AddVariable(Name.String, Type, Register);

	return InputShape;
}

const SShape* FStatementAssignation::GenerateCode(const SCompilingData& Data, const SShape* InputShape)
{
	auto Variable = Data.Scope->FindVariable(Name.String);
	if (!Variable)
	{
		Data.Program->AddError(Name.Location, string("変数'") + Name.String + "'は宣言されていません。。");
		return InputShape;
	}

	if (!Variable->Type.Equals(Expression->GetType()))
	{
		auto Message = "型'" + Variable->Type.ToString() + "'である'" + Name.String + "'に型'" + Expression->GetType().ToString() + "'の値は代入できません。";
		Data.Program->AddError(Operator.Location, Message);
		return InputShape;
	}

	auto Result = Expression->GenerateCodeOrLiteral(Data, Variable->Register);

	if (Result.IsLiteralVaild)
	{
		Variable->Literal = Result.Literal;
		Variable->IsLiteralVaild = true;
	}
	else
	{
		Expression->GenerateCodeOrMove(Data, Variable->Register);
		Variable->IsLiteralVaild = false;
	}

	return InputShape;
}

const SShape* FStatementIf::GenerateCode(const SCompilingData& Data, const SShape* InputShape)
{
	auto StackPointer = Data.Scope->GetStackPointer();
	auto EndLabel = Data.Program->IssueLabel();
	const SShape* FirstOutputShape;
	SLocation FirstLocation;
	bool IsFirst = true;
	bool IsWrong = false;

	for (auto& ElseIfBlock : CaseBlocks)
	{
		auto ConditionRegister = ElseIfBlock->Condition->GenerateCode(Data, StackPointer);
		auto BlockEndLabel = Data.Program->IssueLabel();
		Data.Routine->AddInstruction(EInstruction::IFNJMP, BlockEndLabel, ConditionRegister, 0);
		auto BlockOutputShape = ElseIfBlock->Block->GenerateCode(Data, InputShape);
		Data.Routine->AddInstruction(EInstruction::JMP, EndLabel, 0, 0);
		Data.Routine->AddInstruction(EInstruction::LABEL,BlockEndLabel, 0, 0);

		if (IsFirst)
		{
			FirstOutputShape = BlockOutputShape;
			FirstLocation = ElseIfBlock->IfOrElse;
			IsFirst = false;
		}
		else
		{
			IsWrong = IsWrong || FirstOutputShape != BlockOutputShape;
		}
	}

	if (ElseBlock)
	{
		auto BlockOutputShape = ElseBlock->GenerateCode(Data, InputShape);
		IsWrong = IsWrong || FirstOutputShape != BlockOutputShape;
	}

	if (IsWrong)
	{
		Data.Program->AddError(FirstLocation, "ブロック終了時の形状が統一されていません。");
		return FirstOutputShape;
	}

	Data.Routine->AddInstruction(EInstruction::LABEL, EndLabel, 0, 0);

	return FirstOutputShape;
}

const SShape* FStatementWhile::GenerateCode(const SCompilingData& Data, const SShape* InputShape)
{
	auto StartLabel = Data.Program->IssueLabel();
	auto EndLabel = Data.Program->IssueLabel();
	Data.Routine->AddInstruction(EInstruction::LABEL, StartLabel, 0, 0);
	auto Register = Expression->GenerateCode(Data, Data.Scope->GetStackPointer());
	Data.Routine->AddInstruction(EInstruction::IFNJMP, EndLabel, Register, 0);
	auto BlockOutputShape = Block->GenerateCode(Data, InputShape);
	Data.Routine->AddInstruction(EInstruction::JMP, StartLabel, 0, 0);
	Data.Routine->AddInstruction(EInstruction::LABEL, EndLabel, 0 ,0);

	if (BlockOutputShape != InputShape)
	{
		Data.Program->AddError(WhileLocation, "開始時と終了時の形状が異なるためループできません。");
	}

	return InputShape;
}

const SShape* FStatementReturn::GenerateCode(const SCompilingData& Data, const SShape* InputShape)
{
	if (Expression)
	{
		auto Register = Data.Scope->GetStackPointer();
		Expression->GenerateCodeOrMove(Data, Register);
		Data.Routine->AddInstruction(EInstruction::STORER, Expression->GetType(), Register, 0, 0);
	}
	Data.Routine->AddInstruction(EInstruction::RETURN, 0, 0, 0);

	return InputShape;
}

bool ProcessForkCases
( 
	string Name, 
	const SCompilingData& Data,
	const SProcedure& Procedure,
	const vector<SForkBlock>& ForkCases, 
	vector<FExpression*>* ForkArguments, 
	vector<FStatementBlock*>* ForkBlocks
)
{
	if (Procedure.IsTagNotValue)
	{
		ForkBlocks->resize(Procedure.ForkTagYield.size());
		for (auto& ForkBlock : *ForkBlocks)
			ForkBlock = nullptr;
	}

	for (auto& ForkCase : ForkCases)
	{
		if (Procedure.IsTagNotValue != ForkCase.IsTagNotValue)
		{
			auto Type = ForkCase.IsTagNotValue ? "タグ" : "値";
			auto Message = string("プロシージャ'") + Name + "'の分岐引数に" + Type + "を指定することはできません。";
			Data.Program->AddError(ForkCase.Tag.Location, Message);
		}
		else if (Procedure.IsTagNotValue)
		{
			auto SpecifiedTag = ForkCase.Tag.String;
			auto Match = [SpecifiedTag](pair<string, const SShape*> Source) { return Source.first == SpecifiedTag; };
			auto Iter = find_if(Procedure.ForkTagYield.begin(), Procedure.ForkTagYield.end(), Match);
			auto Index = distance(Procedure.ForkTagYield.begin(), Iter);

			if (Index >= Procedure.ForkTagYield.size())
			{
				auto Message = string("プロシージャ'") + Name + "'の分岐引数にタグ'" + SpecifiedTag + "'を指定することはできません。";
				Data.Program->AddError(ForkCase.Tag.Location, Message);
				return false;
			}

			(*ForkBlocks)[Index] = ForkCase.Block;
		}
		else
		{
			auto DefinedType = Procedure.ForkValueType;
			auto SpecifiedType = ForkCase.Condition->GetType();

			if (!DefinedType.Equals(SpecifiedType))
			{
				auto Message = string("プロシージャ'") + Name + "'の分岐引数に型'" + SpecifiedType.ToString() + "'を指定することはできません。";
				Message += "型'" + DefinedType.ToString() + "'を指定してください。";
				Data.Program->AddError(ForkCase.Tag.Location, Message);
				return false;
			}
			
			ForkArguments->push_back(ForkCase.Condition);
			ForkBlocks->push_back(ForkCase.Block);
		}
	}

	return true;
}

void GenerateCodeArgumentsWithForks
(
	const SCompilingData& Data, 
	const SProcedure& Procedure, 
	const vector<FExpression*>& Arguments,
	const vector<FExpression*>& ForkArguments
)
{
	auto ArgumentRegister = GenerateCodeStoreArguments(Data, Arguments, Data.Scope->GetStackPointer());
	if (!Procedure.IsTagNotValue)
		GenerateCodeStoreArguments(Data, ForkArguments, ArgumentRegister);
}

void GenerateCodeForkBlocks(const SCompilingData& Data, const SProcedure& Procedure, const vector<FStatementBlock*>& ForkBlocks)
{
	uint RegisterOffset = 0;

	for (auto& Yield : Procedure.ForkTagYield)
		RegisterOffset += Yield.second->Size;

	for (uint i = ForkBlocks.size() - 1; i >= 0; i++)
	{
		RegisterOffset -= Procedure.ForkTagYield[i].second->Size;

		if (ForkBlocks[i])
		{
			SCompilingSubscopeEntity Entity(Data, RegisterOffset);
			ForkBlocks[i]->GenerateCode(Entity.Data, Procedure.ForkTagYield[i].second);
		}
	}
}

const SShape* FStatementProcedure::GenerateCode(const SCompilingData& Data, const SShape* InputShape)
{
	auto Procedure = Data.Scope->FindProcedure(Name.String);
	if (!Procedure)
	{
		Data.Program->AddError(Name.Location, string("プロシージャ'") + Name.String + "'は定義されていません。");
		return InputShape;
	}

	if (Procedure->IsBuiltInNotDefine)
	{
		auto ArgumentRegisters = GenerateCodeArguments(Data, Arguments);
		Procedure->GenerateCode(Data, ArgumentRegisters);
	}
	else
	{
		vector<FExpression*> ForkArguments;
		vector<FStatementBlock*> ForkBlocks;
		if (!ProcessForkCases(Name.String, Data, *Procedure, ForkCases, &ForkArguments, &ForkBlocks))
			return InputShape;

		GenerateCodeArgumentsWithForks(Data, *Procedure, Arguments, ForkArguments);
		Data.Routine->AddInstruction(EInstruction::CALL, Procedure->Label, Data.Scope->GetStackPointer(), 0);
		GenerateCodeForkBlocks(Data, *Procedure, ForkBlocks);
	}

	CheckArguments("プロシージャ", Name.String, Data, Procedure->ArgumentTypes, Arguments);

	return InputShape;
}