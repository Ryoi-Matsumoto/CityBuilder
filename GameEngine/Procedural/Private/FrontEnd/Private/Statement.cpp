#include "Statement.h"
#include "Expression.h"
#include "Utility.h"

struct SCompilingSubscopeEntity
{
	SCompilingData Data;
	FScope Subscope;

	SCompilingSubscopeEntity(const SCompilingData& InData, regptr RegisterOffset = 0) : Data(InData), Subscope(InData.Scope, RegisterOffset)
	{
		Data.Scope = &Subscope;
	}
};

struct SCompilingSubroutineEntity : public SCompilingSubscopeEntity
{
	FRoutine Subroutine;

	SCompilingSubroutineEntity(const SCompilingData& InData, SProcedure* CurrentProcedure) 
		: SCompilingSubscopeEntity(InData), Subroutine(CurrentProcedure)
	{
		Data.Routine = &Subroutine;
	}

	void Store()
	{
		Data.Routine->StoreRoutine(Subroutine);
	}
};

SShape* FStatementBlock::GenerateCode(const SCompilingData& Data, SShape* InputShape)
{
	SCompilingSubscopeEntity Entity(Data);
	SShape* OutputShape = InputShape;
	for (auto Statement : Statements)
	{
		OutputShape = Statement->GenerateCode(Entity.Data, OutputShape);
	}
	return OutputShape;
}

SShape* FStatementDefineFunction::GenerateCode(const SCompilingData& Data, SShape* InputShape)
{
	SCompilingSubroutineEntity Entity(Data, nullptr);

	auto Label = Data.Intermediate->IssueLabel();

	// Add a function
	vector<EValueType> ArgumentTypes;
	for (auto Argument : Arguments)ArgumentTypes.push_back(Argument->GetType());
	if (!Data.Scope->AddFunction(Name.String, ReturnType, ArgumentTypes, Block.get(), Label))
	{
		Data.Intermediate->AddError(Name.Location, string("関数'") + Name.String + "'は既に定義されています。");
	}

	// Label
	Entity.Data.Routine->AddInstruction(EInstruction::LABEL, EOperationType::None, Label, 0, 0);

	// Declare arguments
	for (auto Argument : Arguments)Argument->GenerateCode(Entity.Data, nullptr);
	Block->GenerateCode(Entity.Data, nullptr);

	// Return
	Entity.Data.Routine->AddInstruction(EInstruction::RETURN, EOperationType::None, 0, 0, 0);


	Entity.Store();

	return InputShape;
}

SShape* FStatementDefineProcedure::GenerateCode(const SCompilingData& Data, SShape* InputShape)
{
	auto Shape = Data.Scope->FindShape(TargetShape.String);
	auto Label = Data.Intermediate->IssueLabel();

	// Add a procedure
	vector<EValueType> ArgumentTypes;
	for (auto Argument : Arguments)ArgumentTypes.push_back(Argument->GetType());
	auto CurrentProcedure = Data.Scope->AddProcedure(Name.String, Shape, Label, ArgumentTypes, Block.get());
	if (!CurrentProcedure)
	{
		Data.Intermediate->AddError(Name.Location, string("プロシージャ'") + Name.String + "'は既に定義されています。");
	}

	SCompilingSubroutineEntity Entity(Data, CurrentProcedure);

	// Label
	Entity.Data.Routine->AddInstruction(EInstruction::LABEL, EOperationType::None, Label, 0, 0);

	// Declare members of the shape
	if (Shape)
	{
		for (auto Member : Shape->Members)
		{
			FStatementDeclaration ShapeMemberDeclaration(Member.first, Member.second);
			ShapeMemberDeclaration.GenerateCode(Entity.Data, nullptr);
		}
	}
	else Entity.Data.Intermediate->AddError(TargetShape.Location, string("形状'") + TargetShape.String + "'は定義されていません。");

	// Declare arguments
	for (auto Argument : Arguments)Argument->GenerateCode(Entity.Data, nullptr);
	auto OutputShape = Block->GenerateCode(Entity.Data, Data.Scope->FindShape(TargetShape.String));

	// Return
	Entity.Data.Routine->AddInstruction(EInstruction::RETURN, EOperationType::None, 0, 0, 0);

	Entity.Store();

	return OutputShape;
}

SShape* FStatementYield::GenerateCode(const SCompilingData& Data, SShape* InputShape)
{
	auto Procedure = Data.Routine->GetCurrentProcedure();
	if (!Procedure)
	{
		Data.Intermediate->AddError(Value->GetLocation(), "プロシージャ以外ではYieldステートメントは使用できません。");
	}

	if (Type == EProcedureType::Tag)
	{
		auto YieldedShape = Block->GenerateCode(Data, nullptr);
		Data.Routine->AddTagYield(Tag.String, YieldedShape);
		Block->GenerateCode(Data, nullptr);
		
		Procedure->ForkTagYield.push_back(pair<string, SShape*>(Tag.String, YieldedShape));
	}
	else if (Type == EProcedureType::Value)
	{
		if (Value->GetType() != EValueType::Int)
		{
			Data.Intermediate->AddError(Value->GetLocation(), "Yieldステートメントにはタグもしくは整数型以外指定できません。");
		}
		auto YieldedShape = Block->GenerateCode(Data, nullptr);
		Procedure->ForkValueYield = YieldedShape;
		Data.Scope->ReserveRegister(YieldedShape->Size);

		auto StackPointer = Data.Scope->GetStackPointer();
		regptr RegisterOffset = 0;
		for (auto Member : YieldedShape->Members)
		{
			auto OperationType = GetOperationType(Member.first);
			auto IndexRegister = Value->GenerateCode(Data, StackPointer + YieldedShape->Size);
			Data.Routine->AddInstruction(EInstruction::STORERA, OperationType, StackPointer, IndexRegister, 0);
		}

		Data.Scope->ReleaseRegister(YieldedShape->Size);
	}
	else assert(false);
}

SShape* FStatementDefineShape::GenerateCode(const SCompilingData& Data, SShape* InputShape)
{
	Data.Scope->AddShape(Name.String, Members);

	return InputShape;
}

SShape* FStatementDeclaration::GenerateCode(const SCompilingData& Data, SShape* InputShape)
{
	auto Register = Data.Scope->GetStackPointer();
	if (Expression)Expression->GenerateCode(Data, Register);
	Data.Scope->AddVariable(Name.String, Type, Register);

	return InputShape;
}

SShape* FStatementAssignation::GenerateCode(const SCompilingData& Data, SShape* InputShape)
{
	auto Variable = Data.Scope->FindVariable(Name.String);
	if (Variable)
	{
		if (Variable->Type != Expression->GetType())
		{
			auto TypeL = GetTypeName(Variable->Type);
			auto TypeR = GetTypeName(Expression->GetType());
			Data.Intermediate->AddError(Operator.Location, "型'" + TypeL + "'である'" + Name.String + "'に型'" + TypeR + "'の値は代入できません。");
		}
		else
		{
			auto Literal = Expression->GetLiteral(Data);
			if(Literal.IsValid)Variable->Literal = Literal;
			else Expression->GenerateCodeOrMove(Data, Variable->Register);
		}
	}
	else
	{
		Data.Intermediate->AddError(Name.Location, string("'") + Name.String + "'は宣言されていない変数です。");
	}
	
	return InputShape;
}

SShape* FStatementIf::GenerateCode(const SCompilingData& Data, SShape* InputShape)
{
	auto StackPointer = Data.Scope->GetStackPointer();
	auto EndLabel = Data.Intermediate->IssueLabel();
	SShape* FirstOutputShape;
	SLocation FirstLocation;
	bool IsFirst = true;
	bool IsWrong = false;

	for (auto& ElseIfBlock : CaseBlocks)
	{
		auto ConditionRegister = ElseIfBlock->Condition->GenerateCode(Data, StackPointer);
		auto BlockEndLabel = Data.Intermediate->IssueLabel();
		Data.Routine->AddInstruction(EInstruction::IFNJMP, EOperationType::None, BlockEndLabel, ConditionRegister, 0);
		auto BlockOutputShape = ElseIfBlock->Block->GenerateCode(Data, InputShape);
		Data.Routine->AddInstruction(EInstruction::JMP, EOperationType::None, EndLabel, 0, 0);
		Data.Routine->AddInstruction(EInstruction::LABEL, EOperationType::None, BlockEndLabel, 0, 0);

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
		Data.Intermediate->AddError(FirstLocation, "ブロック終了時の形状が統一されていません。");
	}

	Data.Routine->AddInstruction(EInstruction::LABEL, EOperationType::None, EndLabel, 0, 0);

	return FirstOutputShape;
}

SShape* FStatementWhile::GenerateCode(const SCompilingData& Data, SShape* InputShape)
{
	auto StartLabel = Data.Intermediate->IssueLabel();
	auto EndLabel = Data.Intermediate->IssueLabel();
	Data.Routine->AddInstruction(EInstruction::LABEL, EOperationType::None, StartLabel, 0, 0);
	auto Register = Expression->GenerateCode(Data, Data.Scope->GetStackPointer());
	Data.Routine->AddInstruction(EInstruction::IFNJMP, EOperationType::None, EndLabel, Register, 0);
	auto BlockOutputShape = Block->GenerateCode(Data, InputShape);
	Data.Routine->AddInstruction(EInstruction::JMP, EOperationType::None, StartLabel, 0, 0);
	Data.Routine->AddInstruction(EInstruction::LABEL, EOperationType::None, EndLabel, 0 ,0);

	if (BlockOutputShape != InputShape)
	{
		Data.Intermediate->AddError(WhileLocation, "開始時と終了時の形状が異なるためループできません。");
	}

	return InputShape;
}

SShape* FStatementReturn::GenerateCode(const SCompilingData& Data, SShape* InputShape)
{
	if (Expression)
	{
		auto Register = Data.Scope->GetStackPointer();
		Expression->GenerateCodeOrMove(Data, Register);
		Data.Routine->AddInstruction(EInstruction::STORER, GetOperationType(Expression->GetType()), Register, 0, 0);
	}
	Data.Routine->AddInstruction(EInstruction::RETURN, EOperationType::None, 0, 0, 0);

	return InputShape;
}

SShape* FStatementProcedure::GenerateCode(const SCompilingData& Data, SShape* InputShape)
{
	auto Procedure = Data.Scope->FindProcedure(Name.String);

	vector<FExpression*> ForkArguments;
	vector<FStatementBlock*> ForkBlocks;

	if (Procedure->Type == EProcedureType::Tag)
	{
		ForkBlocks.resize(Procedure->ForkTagYield.size());
	}
	
	for (auto& ForkCase : ForkCases)
	{
		if (Procedure->Type != ForkCase.Type)
		{
			auto Type = ForkCase.Type == EProcedureType::Tag ? "タグ" : "値";
			auto Message = string("プロシージャ'") + Name.String + "'の分岐引数に" + Type + "を指定することはできません。";
			Data.Intermediate->AddError(ForkCase.Tag.Location, Message);
		}		
		else if (Procedure->Type == EProcedureType::Tag)
		{
			auto SpecifiedTag = ForkCase.Tag.String;
			auto Match = [SpecifiedTag](pair<string, SShape*> Source) { return Source.first == SpecifiedTag; };
			auto Iter = find_if(Procedure->ForkTagYield.begin(), Procedure->ForkTagYield.end(), Match);
			auto Index = distance(Procedure->ForkTagYield.begin(), Iter);

			if (Index < Procedure->ForkTagYield.size())
			{
				ForkBlocks[Index] = ForkCase.Block;
			}
			else
			{
				auto Message = string("プロシージャ'") + Name.String + "'の分岐引数にタグ'" + SpecifiedTag + "'を指定することはできません。";
				Data.Intermediate->AddError(ForkCase.Tag.Location, Message);
			}
		}
		else if (Procedure->Type == EProcedureType::Value)
		{
			auto DefinedType = Procedure->ForkValueType;
			auto SpecifiedType = ForkCase.Condition->GetType();
			if (DefinedType == SpecifiedType)
			{
				ForkArguments.push_back(ForkCase.Condition);
				ForkBlocks.push_back(ForkCase.Block);
			}
			else
			{
				auto Message = string("プロシージャ'") + Name.String + "'の分岐引数に型'" + GetTypeName(SpecifiedType) + "'を指定することはできません。";
				Message += "型'" + GetTypeName(DefinedType) + "'を指定してください。";
				Data.Intermediate->AddError(ForkCase.Tag.Location, Message);				
			}
		}
		else assert(false);
	}

	// Store arguments
	auto StackPointer = Data.Scope->GetStackPointer();
	auto ArgumentRegister = GenerateCodeStoreArguments(Data, Arguments, StackPointer, 0);

	if (Procedure->Type == EProcedureType::Value)
	{
		GenerateCodeStoreArguments(Data, ForkArguments, ArgumentRegister, Arguments.size());
	}

	// Call
	Data.Routine->AddInstruction(EInstruction::CALL, EOperationType::None, Procedure->Label, StackPointer, 0);

	// Fork
	uint RegisterOffset = 0;
	for (auto& Yield : Procedure->ForkTagYield)RegisterOffset += Yield.second->Size;
	for (uint i = ForkBlocks.size() - 1; i >= 0; i++)
	{
		RegisterOffset -= Procedure->ForkTagYield[i].second->Size;
		if (ForkBlocks[i])
		{
			SCompilingSubscopeEntity Entity(Data, RegisterOffset);
			ForkBlocks[i]->GenerateCode(Entity.Data, Procedure->ForkTagYield[i].second);
		}
	}

	return InputShape;
}