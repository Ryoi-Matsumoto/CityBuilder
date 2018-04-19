#include "Utility.h"
#include "Expression.h"

vector<regptr> GenerateCodeArguments(const SCompilingData& Data, const vector<FExpression*> Arguments)
{
	vector<regptr> Result;
	Result.reserve(Arguments.size());
	for (auto Argument : Arguments)
		Result.push_back(Argument->GenerateCode(Data, Data.Scope->GetStackPointer()));
	return Result;
}

regptr GenerateCodeStoreArguments(const SCompilingData& Data, const vector<FExpression*> Arguments, regptr ArgumentRegister)
{
	for (auto Argument : Arguments)
	{
		auto ResultRegister = Argument->GenerateCode(Data, ArgumentRegister);
		if (ResultRegister != ArgumentRegister)
			ArgumentRegister += Argument->GetType().GetSize();

		Data.Routine->AddInstruction(EInstruction::STOREA, Argument->GetType(), ArgumentRegister, 0, 0);
	}

	return ArgumentRegister;
}

bool CheckArguments(string Type, string Name, const SCompilingData& Data, const vector<SType>& ArgumentTypes, const vector<FExpression*>& Arguments)
{
	auto SpecifiedSize = Arguments.size();
	auto DefinedSize = ArgumentTypes.size();

	if (SpecifiedSize != DefinedSize)
	{
		auto Location = SLocation::Between((*Arguments.begin())->GetLocation(), (*(Arguments.end() - 1))->GetLocation());
		auto Message = Type + "'" + Name + "'��" + to_string(SpecifiedSize) + "�̈������w��ł��܂���B" + to_string(DefinedSize) + "�̈������w�肵�Ă��������B";
		Data.Program->AddError(Location, Message);
		return false;
	}

	for (uint i = 0; i < Arguments.size(); i++)
	{
		auto SpecifiedType = Arguments[i]->GetType();
		auto DefinedType = ArgumentTypes[i];

		if (!SpecifiedType.Equals(DefinedType))
		{
			auto Message = "�^'" + SpecifiedType.ToString() + "'��" + Type + "'" + Name + "'�̑�" + to_string(i + 1) + "�����Ɏw��ł��܂���B�^'" + DefinedType.ToString() + "'���w�肵�Ă��������B";
			Data.Program->AddError(Arguments[i]->GetLocation(), Message);
			return false;
		}
	}

	return true;
}
