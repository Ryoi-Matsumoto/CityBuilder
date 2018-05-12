#include "UIFormula.h"
#include "UIFormulaParameter.h"

enum class EFontType
{
	Normal, Italic, Symbol, Operator
};

class FUIElement
{
public:
	int2 Size;
	int2 Offset;
	uint CenterLine;

private:
	string GetFontName(EFontType FontType)
	{
		switch (FontType)
		{
		case EFontType::Normal:
			return "cmr10";
		case EFontType::Italic:
			return "cmmi10";
		case EFontType::Symbol:
			return "cmex10";
		case EFontType::Operator:
			return "cmsy10";
		default:
			assert(false);
			return "";
		}
	}

protected:
	FRFontSet* GetFontSet(FRFontManager* FontManager, uint FontSize, EFontType Type)
	{
		SRFontType FontType;
		FontType.Size = FontSize;
		FontType.Family = GetFontName(Type);
		return FontManager->GetFontSet(FontType);
	}

public:
	FUIElement() : Size(0), CenterLine(0), Offset(0) {}

	virtual void Render(FR2DRenderer* Renderer, int2 Offset, float4 Color) = 0;

	void Locate(int2 InOffset)
	{
		assert(Size != 0);
		Offset = InOffset;
	}
};

class FUIBar : public FUIElement
{
public:
	FUIBar(FRFontManager* FontManager, int2 BarSize)
	{
		Size = BarSize;
		CenterLine = Size.Y / 2.0f;
	}

	void Render(FR2DRenderer* Renderer, int2 InOffset, float4 Color) override
	{
		Renderer->DrawRectangle(intrect(Offset + InOffset - int2(0, CenterLine), Size), Color);
	}
};

enum class EMathSymbol
{
	Parentheses, CurlyBracket, SquareBracket, AngleBracket, Integral, Sum, Product,
	_OperatorCode,
	Minus = 161, Dot, Closs, Asterisk, Division, _, PlusMinus, MinusPlus, Approx = 188,
	Plus, Equal
};

class FUISymbol : public FUIElement
{
private:
	FRFontLetter* FontLetter;

private:
	// Font : cmex10
	// http://www.math.union.edu/~dpvc/jsmath/symbols/cmex10.html
	enum class EMathSymbolCode : wchar_t
	{
		SmallLeftParentheses = 161,
		SmallRightParentheses,
		SmallLeftSquareBracket,
		SmallRightSquareBracket,
		SmallLowerLeftSquareBracket,
		SmallLowerRightSquareBracket,
		SmallUpperLeftSquareBracket,
		SmallUpperRightSquareBracket,
		SmallLeftCurlyBracket,
		SmallRightCurlyBracket,
		SmallLeftAngleBracket,
		SmallRightAngleBracket,
		_0,
		_1,
		_2,
		_3,

		MediumLeftParentheses,
		MediumRightParentheses,
		MediumLargeLeftParentheses,
		MediumLargeRightParentheses,
		MediumLeftSquareBracket,
		MediumRightSquareBracket,
		MediumLowerLeftSquareBracket,
		MediumLowerRightSquareBracket,
		MediumUpperLeftSquareBracket,
		MediumUpperRightSquareBracket,
		MediumLeftCurlyBracket,
		MediumRightCurlyBracket,
		MediamLeftAngleBracket,
		MediamRightAngleBracket,
		_4,
		_5,

		LargeLeftParentheses,
		LargeRightParentheses,
		LargeLeftSquareBracket,
		LargeRightSquareBracket,
		LargeLowerLeftSquareBracket,
		LargeLowerRightSquareBracket,
		LargeUpperLeftSquareBracket,
		LargeUpperRightSquareBracket,
		LargeLeftCurlyBracket,
		LargeRightCurlyBracket,
		LargeLeftAngleBracket,
		LargeRightAngleBracket,
		_6,
		_7,
		_8,
		_9,

		UpperLeftParentheses = 161 + 0x30,
		UpperRightParentheses,
		LowerLeftParentheses = 161 + 0x40,
		LowerRightParentheses,

		UpperLeftCurlyBracket = 161 + 0x38,
		UpperRightCurlyBracket,
		LowerLeftCurlyBracket,
		LowerRightCurlyBracket,
		MiddleLeftCurlyBracket,
		MiddleRightCurlyBracket,

		Integral = 82,
		Sum = 161 + 0x58,
		Product,
		SmallRadicalSymbol = 161 + 0x70,
		MediumRadicalSymbol,
		LargeRadicalSymbol,
		XLargeRadicalSymbol,
		VerticalRadicalSymbol,
	};

	EMathSymbolCode GetMathSymbolCode(EMathSymbol MathSymbol, uint FontSize, uint ElementSize, bool IsLeftNotRight)
	{
		switch (MathSymbol)
		{
		case EMathSymbol::Parentheses:
			if (IsLeftNotRight)
				return EMathSymbolCode::SmallLeftParentheses;
			else
				return EMathSymbolCode::SmallRightParentheses;
		case EMathSymbol::CurlyBracket:
			if (IsLeftNotRight)
				return EMathSymbolCode::SmallLeftParentheses;
			else
				return EMathSymbolCode::SmallRightParentheses;
		case EMathSymbol::SquareBracket:
			if (IsLeftNotRight)
				return EMathSymbolCode::SmallLeftParentheses;
			else
				return EMathSymbolCode::SmallRightParentheses;
		case EMathSymbol::AngleBracket:
			if (IsLeftNotRight)
				return EMathSymbolCode::SmallLeftParentheses;
			else
				return EMathSymbolCode::SmallRightParentheses;
		case EMathSymbol::Integral:
			return EMathSymbolCode::Integral;
		case EMathSymbol::Sum:
			return EMathSymbolCode::Sum;
		case EMathSymbol::Product:
			return EMathSymbolCode::Product;
		default:
			assert(false);
			return EMathSymbolCode::Sum;
		}
	}

public:
	FUISymbol(FRFontManager* FontManager, EMathSymbol MathSymbol, uint FontSize, uint ElementSize = 0, bool IsLeftNotRight = false)
	{
		// https://github.com/ForNeVeR/wpf-math/blob/master/src/WpfMath/Data/DefaultTexFont.xml
		if (MathSymbol < EMathSymbol::_OperatorCode)
		{
			auto Code = GetMathSymbolCode(MathSymbol, FontSize, ElementSize, IsLeftNotRight);
			auto FontSet = GetFontSet(FontManager, FontSize, EFontType::Symbol);
			FontLetter = FontSet->GetFontLetter((wchar_t)Code);
		}
		else if (MathSymbol == EMathSymbol::Plus)
		{
			auto FontSet = GetFontSet(FontManager, FontSize, EFontType::Normal);
			FontLetter = FontSet->GetFontLetter(L'+');
		}
		else if (MathSymbol == EMathSymbol::Equal)
		{
			auto FontSet = GetFontSet(FontManager, FontSize, EFontType::Normal);
			FontLetter = FontSet->GetFontLetter(L'=');
		}
		else
		{
			auto FontSet = GetFontSet(FontManager, FontSize * 2, EFontType::Operator); 
			FontLetter = FontSet->GetFontLetter((wchar_t)MathSymbol);
		}
		Size = FontLetter->GetBlackBoxSize();
		CenterLine = Size.Y / 2 + FontLetter->GetOriginPoint().Y;
	}

	FUISymbol(FRFontManager* FontManager, wchar_t MathSymbol, uint FontSize, bool IsItalic = false)
	{
		auto FontSet = GetFontSet(FontManager, FontSize, IsItalic ? EFontType::Italic : EFontType::Normal);
		FontLetter = FontSet->GetFontLetter(MathSymbol);
		Size = int2(FontLetter->GetWidth(), FontLetter->GetHeight());
		CenterLine = Size.Y * 0.6;
	}

	void Render(FR2DRenderer* Renderer, int2 InOffset, float4 Color) override
	{
		FontLetter->Render(Renderer, Offset + InOffset - int2(0, CenterLine), Color);
	}
};

class FUIContainer : public FUIElement
{
private:
	FRFontManager* FontManager;

protected:
	vector<unique_ptr<FUIElement>> Elements;

protected:
	template<typename T, typename... TArgs>
	FUIElement* AddElement(TArgs&&... Args)
	{
		auto Element = new T(FontManager, Args...);
		Elements.emplace_back(Element);
		return Element;
	}

public:
	FUIContainer(FRFontManager* FontManager) : FontManager(FontManager) {}

	void Render(FR2DRenderer* Renderer, int2 InOffset, float4 Color) override
	{
		for (auto& Element : Elements)
			Element->Render(Renderer, InOffset + Offset, Color);
	}
};

class FUIExpression : public FUIContainer
{
private:
	const FExpression* Expression;
	bool IsConstant;

private:
	wchar_t ToGreekCharactor(string Name)
	{
		static vector<uint64> Greeks =
		{ 
			"alpha"u64,
			"beta"u64,
			"gamma"u64,
			"delta"u64,
			"epsilon"u64,
			"zeta"u64,
			"eta"u64,
			"theta"u64,
			"iota"u64,
			"kappa"u64,
			"lambda"u64,
			"mu"u64,
			"nu"u64,
			"xi"u64,
			"pi"u64,
			"rho"u64,
			"sigma"u64,
			"tau"u64,
			"upsilon"u64,
			"phi"u64,
			"chi"u64,
			"psi"u64
		};

		static vector<uint64> SpecialGreeks =
		{
			"omega"u64,
			"varepsilon"u64,
			"vartheta"u64,
			"varpi"u64,
			"varrho"u64
		};

		auto Code = ToUint64(Name.data());
		auto Itr = find(Greeks.begin(), Greeks.end(), Code);
		if (Itr != Greeks.end())
			return 174 + (Itr - Greeks.begin());
		Itr = find(SpecialGreeks.begin(), SpecialGreeks.end(), Code);
		if (Itr != SpecialGreeks.end())
			return 33 + (Itr - SpecialGreeks.begin());
		return Name[0];
	}

	EMathSymbol ToMathSymbol(EExpressionType Type)
	{
		switch (Type)
		{
		case EExpressionType::Add:
			return EMathSymbol::Plus;
		case EExpressionType::Sub:
			return EMathSymbol::Minus;
		case EExpressionType::Mul:
			return EMathSymbol::Dot;
		case EExpressionType::Function:
			return EMathSymbol::Dot;
		default:
			assert(false);
			return EMathSymbol::Asterisk;
		}
	}

	int2 AddOperationUnit(uint FontSize, int2 Offset)
	{
		uint MaxHeight = 0;
		vector<FUIExpression*> TermElements;
		for (auto& Term : Expression->GetElements())
		{
			auto Element = (FUIExpression*)AddElement<FUIExpression>(&Term, FontSize);
			MaxHeight = max(MaxHeight, Element->Size.Y);
			TermElements.push_back(Element);
		}

		auto OperatorCode = ToMathSymbol(Expression->GetType());
		float XOffset = 0;
		bool IsFirst = true;
		auto Space = (float)FontSize * OperatorSpace;
		for (auto& TermElement : TermElements)
		{
			// a mul symbol is necessary when the right side is constant
			if (!IsFirst && !(Expression->GetType() == EExpressionType::Mul && !TermElement->IsConstant))
			{
				XOffset += Space;
				auto Operator = AddElement<FUISymbol>(OperatorCode, FontSize);
				Operator->Locate(Offset + int2(XOffset, 0));
				XOffset += Operator->Size.X + Space;
			}
			TermElement->Locate(Offset + int2(XOffset, 0));
			XOffset += TermElement->Size.X;
			IsFirst = false;
		}
		return int2(XOffset, MaxHeight);
	}

	int2 AddText(wstring Text, uint FontSize, int2 Offset)
	{
		uint XOffset = 0;
		uint Height = 0;
		for (auto Char : Text)
		{
			auto Letter = AddElement<FUISymbol>(Char, FontSize, false);
			Letter->Locate(Offset + int2(XOffset, 0));
			XOffset += Letter->Size.X;
			Height = max(Height, Letter->Size.Y);
		}
		return int2(XOffset, Height);
	}

public:
	FUIExpression(FRFontManager* FontManager, const FExpression* Expression, uint FontSize)
		: FUIContainer(FontManager)
		, Expression(Expression)
		, IsConstant(false)
	{
		CenterLine = 0;
		auto& Terms = Expression->GetElements();
		switch (Expression->GetType())
		{
		case EExpressionType::Empty:
			assert(false);
			break;
		case EExpressionType::NaN:
		{
			Size = AddText(L"undefined", FontSize, 0);
			break;
		}
		case EExpressionType::Function:
			switch (ToUint64(Expression->GetName().data()))
			{
			case "log"u64:
				if (Terms.size() == 2)
				{
					const float BaseScale = 0.7f;
					auto LogSize = AddText(L"log", FontSize, 0);
					auto Base = AddElement<FUIExpression>(&Terms[0], (float)FontSize * BaseScale);
					auto Anti = AddElement<FUIExpression>(&Terms[1], FontSize);

					auto AntiXOffset = LogSize.X + Base->Size.X + (float)FontSize * OperatorSpace;
					Base->Locate(int2(LogSize.X, Base->Size.Y / 2.0f));
					Anti->Locate(int2(AntiXOffset, 0));

					Size = Anti->Size + int2(AntiXOffset, Base->Size.Y / 3.0f);
				}
				break;
			case "integral"u64:
				if (Terms.size() == 2)
				{
					auto Integral = AddElement<FUISymbol>(EMathSymbol::Integral, FontSize * 8);
					auto Delta = AddElement<FUISymbol>(L'd', FontSize, true);
					auto Main = AddElement<FUIExpression>(&Terms[0], FontSize);
					auto Bound = AddElement<FUIExpression>(&Terms[1], FontSize);
					Integral->Locate(0);
					Main->Locate(int2(Integral->Size.X, 0));
					Delta->Locate(int2(Integral->Size.X + Main->Size.X, 0));
					Bound->Locate(int2(Integral->Size.X + Main->Size.X + Delta->Size.X, 0));
					Size = int2(Integral->Size.Y + Main->Size.X + Delta->Size.X + Bound->Size.X, Integral->Size.Y);
				}
				break;
			default:
			{
				auto UnitSize = AddOperationUnit(FontSize, 0);
				auto FunctionSize = AddText(ToWString(Expression->GetName()), FontSize, 0);
				auto LeftBracket = AddElement<FUISymbol>(EMathSymbol::Parentheses, true, UnitSize.Y, FontSize);
				auto RightBracket = AddElement<FUISymbol>(EMathSymbol::Parentheses, false, UnitSize.Y, FontSize);

				LeftBracket->Locate(int2(FunctionSize.X, 0));
				RightBracket->Locate(int2(FunctionSize.X + LeftBracket->Size.X + UnitSize.X, 0));
				Size = int2(FunctionSize.X + LeftBracket->Size.X + UnitSize.X + RightBracket->Size.X, UnitSize.Y);
			}
			}
			break;
		case EExpressionType::Variable:
		case EExpressionType::MathConstant:
		{
			auto Identifier = AddElement<FUISymbol>(ToGreekCharactor(Expression->GetName()), FontSize, true);
			Identifier->Locate(0);
			Size = Identifier->Size;
			break;
		}
		case EExpressionType::Constant:
		{
			IsConstant = true;
			auto IdentSize = AddText(ToWString(Expression->GetValue().str()), FontSize, 0);
			Size = IdentSize;
			break;
		}
		case EExpressionType::Mul:
		case EExpressionType::Add:
		case EExpressionType::Sub:
		{
			Size = AddOperationUnit(FontSize, 0);
			break;
		}
		case EExpressionType::Div:
		{
			IsConstant = true;
			auto Namerator = AddElement<FUIExpression>(&Terms[0], FontSize);
			auto Denominator = AddElement<FUIExpression>(&Terms[1], FontSize);

			auto SpaceWidth = (float)FontSize * FractionSideSpace;
			auto SpaceHeight = (float)FontSize * FractionBarSpace;
			auto BarHeight = (float)FontSize * FractionBarHeigh;

			auto Width = max(Namerator->Size.X, Denominator->Size.X) + SpaceWidth;
			auto Bar = AddElement<FUIBar>(int2(Width, BarHeight));

			Namerator->Locate(int2((Width - Namerator->Size.X) / 2.0f, - (Namerator->Size.Y + SpaceHeight) / 2.0f));
			Denominator->Locate(int2((Width - Denominator->Size.X) / 2.0f, (Denominator->Size.Y + SpaceHeight) / 2.0f));
			Bar->Locate(0);

			Size = int2(Width, Namerator->Size.Y + SpaceHeight + Denominator->Size.Y);
			break;
		}
		case EExpressionType::Pow:
		{
			auto Main = AddElement<FUIExpression>(&Terms[0], FontSize);
			auto Power = AddElement<FUIExpression>(&Terms[1], FontSize * 0.7);
			Main->Locate(0);
			auto YOffset = Power->Size.Y * 0.4;
			Power->Locate(int2(Main->Size.X, YOffset - Main->Size.Y / 2));
			Size = Main->Size + int2(Power->Size.X, Power->Size.Y - YOffset);
			CenterLine = Main->CenterLine;
			break;
		}
		default:
			Size = 1;
			break;
		}
	}
};

class FUIProcess : public FUIContainer, public FUIControl, public FUIScrollable
{
private:
	SUIViewRange* ViewRange;

public:
	FUIProcess(FRFontManager* FontManager, SUIViewRange* ViewRange)
		: FUIContainer(FontManager), ViewRange(ViewRange) {}

	void SetExpressions(vector<const FExpression*> Expressions, vector<uint>* YPositions)
	{
		Elements.clear();
		const uint FontSize = 40;
		uint YOffset = 0;
		bool IsFirst = true;
		for (auto Expression : Expressions)
		{
			auto Element = AddElement<FUIExpression>(Expression, FontSize);

			if (IsFirst)
			{
				Offset = int2(0, Element->Size.Y / 2);
			}
			else
			{
				YPositions->push_back(YOffset);
				auto Equal = AddElement<FUISymbol>(EMathSymbol::Equal, FontSize);
				Equal->Locate(int2(0, YOffset));
			}

			Element->Locate(int2(FontSize, YOffset));
			YOffset += Element->Size.Y + FontSize / 2;
			IsFirst = false;
		}
	}

	void OnRendering() override
	{
		Render(GetRenderer(), 0, 1);
	}
};

class FUIRule : public FUIContainer, public FUIControl
{
private:
	SUIViewRange* ViewRange;

public:
	FUIRule(FRFontManager* FontManager, SUIViewRange* ViewRange) 
		: FUIContainer(FontManager), ViewRange(ViewRange) {}

	void SetRules(vector<const FRule*> Rules, vector<uint> YPositions)
	{
		Elements.clear();
		const uint FontSize = 40;
		for (uint i = 0; i < Rules.size(); i++)
		{
			if (!Rules[i] || Rules[i]->GetBefore().IsEmpty())
				continue;

			auto After = AddElement<FUIExpression>(&Rules[i]->GetBefore(), FontSize);
			auto Before = AddElement<FUIExpression>(&Rules[i]->GetAfter(), FontSize);
			auto Equal = AddElement<FUISymbol>(EMathSymbol::Equal, FontSize);
			After->Locate(int2(0, YPositions[i]));
			Equal->Locate(int2(After->Size.Y, YPositions[i]));
			Before->Locate(int2(After->Size.Y + Equal->Size.Y, YPositions[i]));

			if (i == 0)
				Offset = int2(0, max(After->Size.Y, Before->Size.Y) / 2);
		}
	}

	void OnRendering() override
	{
		Render(GetRenderer(), 0, 1);
	}
};

FUIFormula::FUIFormula()
{
	Rule = FFileManager::ReadTextFile("C:\\Users\\RYOUI\\Desktop\\CityBuilder\\GameEngine\\Math\\Private\\Rule.txt");
}

void FUIFormula::OnInitialize()
{
	ProcessView = new FUIProcess(Application->GetRenderingManager(), &ViewRange);
	RuleView = new FUIRule(Application->GetRenderingManager(), &ViewRange);

	auto Grid = new FUIGrid();
	Grid->AddXDefine(SUILenght(3, true));
	Grid->AddXDefine(SUILenght(1, true));
	{
		ProcessView->SetXIndex(0);
		RuleView->SetXIndex(1);

		Grid->AddChild(ProcessView);
		Grid->AddChild(RuleView);
	}

	SetContent(Grid);
	SetViewRange(&ViewRange);

	FUIScrollView::OnInitialize();
}

void FUIFormula::SetExpression(FExpression const& Source)
{
	if (Source.IsEmpty())
		return;

	Model = Source.Calculate(FRuleTable(Rule));
	AddFontResourceEx(L"latinmodern-math.otf", FR_PRIVATE, nullptr);

	vector<const FExpression*> Expressions;
	vector<const FRule*> Rules;

	Expressions.push_back(&Model.GetExpression());

	for (auto& Process : Model.GetProcesses())
	{
		Expressions.push_back(&Process.Expression);
		Rules.push_back(&Process.Rule);
	}

	vector<uint> YPositions;
	ProcessView->SetExpressions(Expressions, &YPositions);
	RuleView->SetRules(Rules, YPositions);

	Update();
}