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
	int CenterLine;

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
		//assert(Size != 0);
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
	Parentheses, CurlyBracket, SquareBracket, AngleBracket, Root, Integral, Sigma, Product,
	_OperatorCode,
	Infinity = 49, 
	LeftArrow = 195, RightArrow = 33, UpArrow, DownArrow, LeftRightArrow,
	LeftDoubleArrow = 39, RightDoubleArrow, UpDoubleArrow, DownDoubleArrow, LeftRightDoubleArrow,
	Minus = 161, Dot, Closs, Asterisk, Division, _, PlusMinus, MinusPlus, Approx = 188,
	Plus, Equal
};

class FUISymbol : public FUIElement
{
private:
	FRFontLetter* FontLetter;
	uint Adjustment;

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

		MediumLeftParentheses = 179,
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

		LargeLeftParenthese = 195,
		LargeRightParentheses = 33,
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

		UpperLeftParentheses = 48,
		UpperRightParentheses,

		UpperLeftCurlyBracket = 56,
		UpperRightCurlyBracket,
		LowerLeftCurlyBracket,
		LowerRightCurlyBracket,
		MiddleLeftCurlyBracket,
		MiddleRightCurlyBracket,
		LineCurlyBracket,

		LowerLeftParentheses = 62,
		LowerRightParentheses,
		LineLeftParentheses,
		LineRightParentheses,

		Sum = 88,
		Product,
		Integral,

		MediumLargeLeftCurlyBracket = 110,
		MediumLargeRightCurlyBracket,
		SmallRoot,
		MediumRoot,
		MediumLargeRoot,
		LargeRoot,
		VerticalRoot,
	};

	EMathSymbolCode GetMathSymbolCode(float* OutHeight, EMathSymbol MathSymbol, uint FontSize, uint ElementSize, bool IsLeftNotRight)
	{
		const uint SmallSize = ElementSize;
		const uint MediumSize = ElementSize * 1.5f;
		const uint MediumLargeSize = ElementSize * 3;
		*OutHeight = 1;
		switch (MathSymbol)
		{
		case EMathSymbol::Parentheses:
			if (FontSize <= SmallSize)
			{
				*OutHeight = 1.16f;
				if (IsLeftNotRight)
					return EMathSymbolCode::SmallLeftParentheses;
				else
					return EMathSymbolCode::SmallRightParentheses;
			}
			else if(FontSize <= MediumSize)
			{
				*OutHeight = 1.76f;
				if (IsLeftNotRight)
					return EMathSymbolCode::MediumLeftParentheses;
				else
					return EMathSymbolCode::MediumRightParentheses;
			}
			else if(FontSize <= MediumLargeSize)
			{
				*OutHeight = 2.36f;
				if (IsLeftNotRight)
					return EMathSymbolCode::MediumLargeLeftParentheses;
				else
					return EMathSymbolCode::MediumLargeRightParentheses;
			}
			else
			{
				*OutHeight = 2.96f;
				if (IsLeftNotRight)
					return EMathSymbolCode::LargeLeftParenthese;
				else
					return EMathSymbolCode::LargeRightParentheses;
			}
		case EMathSymbol::CurlyBracket:
			if (FontSize <= SmallSize)
			{
				*OutHeight = 1.16f;
				if (IsLeftNotRight)
					return EMathSymbolCode::SmallLeftCurlyBracket;
				else
					return EMathSymbolCode::SmallRightCurlyBracket;
			}
			else if (FontSize <= MediumSize)
			{
				*OutHeight = 1.76f;
				if (IsLeftNotRight)
					return EMathSymbolCode::MediumLeftCurlyBracket;
				else
					return EMathSymbolCode::MediumRightCurlyBracket;
			}
			else if (FontSize <= MediumLargeSize)
			{
				*OutHeight = 2.36f;
				if (IsLeftNotRight)
					return EMathSymbolCode::MediumLargeLeftCurlyBracket;
				else
					return EMathSymbolCode::MediumLargeRightCurlyBracket;
			}
			else
			{
				*OutHeight = 2.96f;
				if (IsLeftNotRight)
					return EMathSymbolCode::LargeLeftCurlyBracket;
				else
					return EMathSymbolCode::LargeRightCurlyBracket;
			}
		case EMathSymbol::SquareBracket:
			if (FontSize <= SmallSize)
			{
				*OutHeight = 1.16f;
				if (IsLeftNotRight)
					return EMathSymbolCode::SmallLeftSquareBracket;
				else
					return EMathSymbolCode::SmallRightSquareBracket;
			}
			else if (FontSize <= MediumSize)
			{
				*OutHeight = 2.36f;
				if (IsLeftNotRight)
					return EMathSymbolCode::MediumLeftSquareBracket;
				else
					return EMathSymbolCode::MediumRightSquareBracket;
			}
			else
			{
				*OutHeight = 2.96f;
				if (IsLeftNotRight)
					return EMathSymbolCode::LargeLeftSquareBracket;
				else
					return EMathSymbolCode::LargeRightSquareBracket;
			}
		case EMathSymbol::AngleBracket:
			if (IsLeftNotRight)
				return EMathSymbolCode::SmallLeftParentheses;
			else
				return EMathSymbolCode::SmallRightParentheses;
		case EMathSymbol::Root:
			if (FontSize <= SmallSize)
			{
				*OutHeight = 1.16f;
				return EMathSymbolCode::SmallRoot;
			}
			else if (FontSize <= MediumSize)
			{
				*OutHeight = 1.76f;
				return EMathSymbolCode::MediumRoot;
			}
			else if (FontSize <= MediumLargeSize)
			{
				*OutHeight = 2.36f;
				return EMathSymbolCode::MediumLargeRoot;
			}
			else
			{
				*OutHeight = 2.96f;
				return EMathSymbolCode::LargeRoot;
			}
		case EMathSymbol::Integral:
			return EMathSymbolCode::Integral;
		case EMathSymbol::Sigma:
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
			float Height;
			auto Code = GetMathSymbolCode(&Height, MathSymbol, FontSize, ElementSize, IsLeftNotRight);
			auto FontSet = GetFontSet(FontManager, (float)FontSize / Height * 4, EFontType::Symbol);
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
		Size = int2(FMath::Max(FontLetter->GetWidth(), FontLetter->GetBlackBoxSize().X), FontLetter->GetBlackBoxSize().Y);
		CenterLine = Size.Y / 2.0f; 
		Adjustment = FontLetter->GetOriginPoint().Y;
	}

	FUISymbol(FRFontManager* FontManager, wchar_t MathSymbol, uint FontSize, bool IsItalic = false)
	{
		auto FontSet = GetFontSet(FontManager, FontSize, IsItalic ? EFontType::Italic : EFontType::Normal);
		FontLetter = FontSet->GetFontLetter(MathSymbol);
		Size = int2(FontLetter->GetWidth(), FontLetter->GetHeight());
		CenterLine = Size.Y / 2.0f;
		Adjustment = Size.Y * 0.2f;
	}

	void Render(FR2DRenderer* Renderer, int2 InOffset, float4 Color) override
	{
		FontLetter->Render(Renderer, Offset + InOffset - int2(0, CenterLine + Adjustment), Color);
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
	auto AddElement(TArgs&&... Args)
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

enum class EOperationLevel
{
	Add, Mul, Pow
};

class FUIExpression : public FUIContainer
{
private:
	const FExpression* Expression;
	bool IsConstant;
	uint BracketIndex;

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

		if (Name.size() > 1)
		{
			auto Code = ToUint64(Name.data());
			auto Itr = find(Greeks.begin(), Greeks.end(), Code);
			if (Itr != Greeks.end())
				return 174 + (Itr - Greeks.begin());
			Itr = find(SpecialGreeks.begin(), SpecialGreeks.end(), Code);
			if (Itr != SpecialGreeks.end())
				return 33 + (Itr - SpecialGreeks.begin());
		}
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

	int2 AddOperationUnit(uint FontSize, int2 Offset, bool IsSmall)
	{
		uint MaxHeight = 0;
		vector<FUIExpression*> TermElements;
		for (auto& Term : Expression->GetElements())
		{
			FUIExpression* Element;
			if(Expression->Type == EExpressionType::Mul && (Term.Type == EExpressionType::Add || Term.Type == EExpressionType::Sub))
				Element = AddElement<FUIExpression>(&Term, FontSize, IsSmall, EOperationLevel::Add);
			else
				Element = AddElement<FUIExpression>(&Term, FontSize, IsSmall);
			MaxHeight = max(MaxHeight, Element->Size.Y);
			TermElements.push_back(Element);
		}

		auto OperatorCode = ToMathSymbol(Expression->GetType());
		uint XOffset = 0, UpperWidth = 0, LowerWidth = 0;
		bool IsFirst = true;
		auto Space = IsSmall ? 0 : (float)FontSize * OperatorSpace;
		for (auto& TermElement : TermElements)
		{
			// a mul symbol is necessary when the right operator is constant
			if (!IsFirst && !(Expression->GetType() == EExpressionType::Mul && !TermElement->IsConstant))
			{
				XOffset += Space;
				auto Operator = AddElement<FUISymbol>(OperatorCode, FontSize);
				Operator->Locate(Offset + int2(XOffset, 0));
				XOffset += Operator->Size.X + Space;
			}
			TermElement->Locate(Offset + int2(XOffset, 0));
			XOffset += TermElement->Size.X;
			UpperWidth = FMath::Max(UpperWidth, TermElement->CenterLine);
			LowerWidth = FMath::Max(LowerWidth, TermElement->Size.Y - TermElement->CenterLine);
			BracketIndex = max(BracketIndex, TermElement->BracketIndex);
			IsFirst = false;
		}
		CenterLine = UpperWidth;
		return int2(XOffset, UpperWidth + LowerWidth);
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
			CenterLine = Letter->CenterLine;
		}
		return int2(XOffset, Height);
	}

	int GetDefaultSize(int FontSize)
	{
		return FontSize * 0.6f;
	}

	int GetArrangeBottomYOffset(FUIElement* Element, int FontSize)
	{
		return Element->Size.Y - GetDefaultSize(FontSize) / 2.0f - Element->CenterLine;
	}

public:
	FUIExpression(FRFontManager* FontManager, const FExpression* Expression, uint FontSize, bool IsSmall)
		: FUIContainer(FontManager)
		, Expression(Expression)
		, IsConstant(false)
		, BracketIndex(0)
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
					auto Base = AddElement<FUIExpression>(&Terms[0], (float)FontSize * BaseScale, true);
					auto Anti = AddElement<FUIExpression>(&Terms[1], FontSize, IsSmall, EOperationLevel::Add);

					auto AntiXOffset = LogSize.X + Base->Size.X + (float)FontSize * OperatorSpace;
					Base->Locate(int2(LogSize.X, Base->Size.Y / 2.0f));
					Anti->Locate(int2(AntiXOffset, 0));

					CenterLine = Anti->CenterLine;
					Size = Anti->Size + int2(AntiXOffset, Base->Size.Y / 3.0f);
					BracketIndex = max(Anti->BracketIndex, Base->BracketIndex);
				}
				break;
			case "int"u64:
				if (Terms.size() == 2)
				{
					auto Integral = AddElement<FUISymbol>(EMathSymbol::Integral, FontSize);
					auto Delta = AddElement<FUISymbol>(L'd', FontSize, true);
					auto Main = AddElement<FUIExpression>(&Terms[0], FontSize, IsSmall);
					auto Bound = AddElement<FUIExpression>(&Terms[1], FontSize, IsSmall);
					Integral->Locate(0);
					Main->Locate(int2(Integral->Size.X, 0));
					Delta->Locate(int2(Integral->Size.X + Main->Size.X, 0));
					Bound->Locate(int2(Integral->Size.X + Main->Size.X + Delta->Size.X, 0));
					Size = int2(Integral->Size.X + Main->Size.X + Delta->Size.X + Bound->Size.X, Integral->Size.Y);
					CenterLine = Integral->CenterLine;
					BracketIndex = max(Main->BracketIndex, Bound->BracketIndex);
				}
				break;
			case "sum"u64:
			{
				if (Terms.size() < 4)
					break;

				const float ArugmentSize = 0.7f;
				auto Space = FontSize * 0.25f;
				auto ArgumentSpace = FontSize * 0.1f;
				auto Sigma = AddElement<FUISymbol>(EMathSymbol::Sigma, FontSize);
				auto Equal = AddElement<FUISymbol>(EMathSymbol::Equal, FontSize * ArugmentSize, true);
				auto Bound = AddElement<FUIExpression>(&Terms[0], FontSize * ArugmentSize, true);
				auto Initial = AddElement<FUIExpression>(&Terms[1], FontSize * ArugmentSize, true);
				auto Lenght = AddElement<FUIExpression>(&Terms[2], FontSize * ArugmentSize, true);
				auto Main = AddElement<FUIExpression>(&Terms[3], FontSize, IsSmall, EOperationLevel::Add);
				auto LowerWidth = Bound->Size.X + Equal->Size.X + Initial->Size.X;
				auto UpperOrigin = FMath::Max(0, (Sigma->Size.X - Lenght->Size.X) / 2.0f);
				auto LowerOrigin = FMath::Max(0, (Sigma->Size.X - LowerWidth) / 2.0f);
				auto UpperY = -(Sigma->Size.Y - Sigma->CenterLine) - (Lenght->Size.Y - Lenght->CenterLine) - ArgumentSpace;
				auto LowerY = Sigma->CenterLine + Initial->CenterLine + ArgumentSpace;
				auto SigmaWidth = FMath::Max(Sigma->Size.X, FMath::Max(Lenght->Size.X, LowerWidth));
				Sigma->Locate(0);
				Lenght->Locate(int2(UpperOrigin, UpperY));
				Bound->Locate(int2(LowerOrigin, LowerY));
				Equal->Locate(int2(LowerOrigin + Bound->Size.X, LowerY + Equal->Size.Y * 0.5f));
				Initial->Locate(int2(LowerOrigin + Bound->Size.X + Equal->Size.X, LowerY));
				Main->Locate(int2(SigmaWidth + Space, 0));
				auto Height = FMath::Max(Sigma->Size.Y + Lenght->Size.Y + Initial->Size.Y, Main->Size.Y);
				Size = int2(SigmaWidth + Space + Main->Size.X, Height);
				CenterLine = FMath::Max(Main->CenterLine, Height / 2.0f);
				BracketIndex = Main->BracketIndex;
				break;
			}
			case "lim"u64:
			{
				if (Terms.size() < 3)
					break;

				const float ArugmentSize = 0.7f;
				auto Space = FontSize * 0.25f;
				auto ArgumentSpace = FontSize * 0.3f;
				auto LimSize = AddText(L"lim", FontSize, 0);
				auto Arrow = AddElement<FUISymbol>(EMathSymbol::RightArrow, FontSize * ArugmentSize , true);
				auto Bound = AddElement<FUIExpression>(&Terms[0], FontSize * ArugmentSize, true);
				auto Limit = AddElement<FUIExpression>(&Terms[1], FontSize * ArugmentSize, true);
				auto Main = AddElement<FUIExpression>(&Terms[2], FontSize, IsSmall, EOperationLevel::Add);
				auto LowerWidth = Bound->Size.X + Arrow->Size.X + Limit->Size.X;
				auto LowerOrigin = (LimSize.X - LowerWidth) / 2.0f;
				auto LowerY = LimSize.Y / 2.0f + ArgumentSpace;
				auto LimWidth = LimSize.X + abs(LowerOrigin);
				Bound->Locate(int2(LowerOrigin, LowerY));
				Arrow->Locate(int2(LowerOrigin + Bound->Size.X, LowerY));
				Limit->Locate(int2(LowerOrigin + Bound->Size.X + Arrow->Size.X, LowerY));
				Main->Locate(int2(LimWidth + Space, 0));
				auto Height = FMath::Max(LimSize.Y + Limit->Size.Y, Main->Size.Y);
				Size = int2(LimWidth + Space + Main->Size.X, Height);
				CenterLine = FMath::Max(Main->CenterLine, Height / 2.0f);
				BracketIndex = Main->BracketIndex;
				break;
			}
			case "sqrt"u64:
			{
				if (Terms.size() == 0)
					break;
				const float BarHeight = FontSize * 0.05f;
				const float Space = FontSize * 0.2f;
				auto Main = AddElement<FUIExpression>(&Terms[0], FontSize, IsSmall);
				auto Root = AddElement<FUISymbol>(EMathSymbol::Root, Main->Size.Y + Space, FontSize);
				auto Bar = AddElement<FUIBar>(int2(Main->Size.X, BarHeight));

				float XOffset = 0, OverHeight = 0;
				if (Terms.size() == 2)
				{
					auto PowerSize = FontSize * 0.7f;
					auto Power = AddElement<FUIExpression>(&Terms[1], PowerSize, true);
					auto YOffset = Root->Size.Y / 3.0f + GetArrangeBottomYOffset(Power, PowerSize);
					Power->Locate(int2(0, -YOffset));
					BracketIndex = max(BracketIndex, Power->BracketIndex);
					XOffset = FMath::Max(Power->Size.X - GetDefaultSize(PowerSize), 0);
					OverHeight = YOffset + Power->CenterLine - Root->CenterLine;
				}

				Root->Locate(int2(XOffset, -Space + FontSize * 0.15f));
				Main->Locate(int2(XOffset + Root->Size.X, Main->CenterLine - Main->Size.Y / 2.0f));
				Bar->Locate(int2(XOffset + Root->Size.X, -Root->CenterLine));
				Size = Root->Size + int2(XOffset + Main->Size.X, OverHeight);
				CenterLine = Root->Size.Y / 2.0f + OverHeight;
				BracketIndex = Main->BracketIndex;
			
				break;
			}
			default:
			{
				auto UnitSize = AddOperationUnit(FontSize, 0, IsSmall);
				auto FunctionSize = AddText(ToWString(Expression->GetName()), FontSize, 0);
				auto LeftBracket = AddElement<FUISymbol>(EMathSymbol::Parentheses, UnitSize.Y, FontSize, true);
				auto RightBracket = AddElement<FUISymbol>(EMathSymbol::Parentheses, UnitSize.Y, FontSize, false);

				LeftBracket->Locate(int2(FunctionSize.X, 0));
				RightBracket->Locate(int2(FunctionSize.X + LeftBracket->Size.X + UnitSize.X, 0));
				Size = int2(FunctionSize.X + LeftBracket->Size.X + UnitSize.X + RightBracket->Size.X, UnitSize.Y);
			}
			}
			break;
		case EExpressionType::Variable:
		case EExpressionType::MathConstant:
		{
			FUISymbol* Identifier;
			if(Expression->GetName() == "infty")
				Identifier = AddElement<FUISymbol>(EMathSymbol::Infinity, FontSize);
			else
				Identifier = AddElement<FUISymbol>(ToGreekCharactor(Expression->GetName()), FontSize, true);
			Identifier->Locate(0);
			Size = Identifier->Size;
			CenterLine = Identifier->CenterLine;
			break;
		}
		case EExpressionType::Constant:
		{
			IsConstant = true;
			Size = AddText(ToWString(Expression->GetValue().str()), FontSize, 0);
			break;
		}
		case EExpressionType::Mul:
		case EExpressionType::Add:
		case EExpressionType::Sub:
		{
			Size = AddOperationUnit(FontSize, 0, IsSmall);
			break;
		}
		case EExpressionType::Div:
		{
			IsConstant = true;
			auto Namerator = AddElement<FUIExpression>(&Terms[0], FontSize, IsSmall);
			auto Denominator = AddElement<FUIExpression>(&Terms[1], FontSize, IsSmall);

			auto SpaceWidth = (float)FontSize * FractionSideSpace;
			auto SpaceHeight = IsSmall ? 0 : (float)FontSize * FractionBarSpace;
			auto BarHeight = (float)FontSize * FractionBarHeigh;

			auto Width = max(Namerator->Size.X, Denominator->Size.X) + SpaceWidth;
			auto Bar = AddElement<FUIBar>(int2(Width, BarHeight));
			auto NameratorY = -(int)(Namerator->Size.Y - Namerator->CenterLine) - SpaceHeight / 2.0f;
			auto DenominatorY = Denominator->CenterLine + SpaceHeight / 2.0f;
			Namerator->Locate(int2((Width - Namerator->Size.X) / 2.0f, NameratorY));
			Denominator->Locate(int2((Width - Denominator->Size.X) / 2.0f, DenominatorY));
			Bar->Locate(0);
			Size = int2(Width, Namerator->Size.Y + SpaceHeight + Denominator->Size.Y);
			CenterLine = Namerator->Size.Y + SpaceHeight / 2.0f;
			BracketIndex = max(Namerator->BracketIndex, Denominator->BracketIndex);
			break;
		}
		case EExpressionType::Pow:
		{
			auto PowerSize = FontSize * 0.7;
			auto Main = AddElement<FUIExpression>(&Terms[0], FontSize, IsSmall, EOperationLevel::Mul);
			auto Power = AddElement<FUIExpression>(&Terms[1], PowerSize, true);
			auto YOffset = PowerSize / 3.0f - GetArrangeBottomYOffset(Power, PowerSize);
			auto OverHeight = FMath::Max(0, Power->CenterLine - YOffset);
			Main->Locate(0);
			Power->Locate(int2(Main->Size.X, YOffset - Main->CenterLine));
			Size = Main->Size + int2(Power->Size.X, OverHeight);
			CenterLine = Main->CenterLine + OverHeight;
			BracketIndex = max(Main->BracketIndex, Power->BracketIndex);
			break;
		}
		default:
			Size = 1;
			break;
		}
	}

	FUIExpression(FRFontManager* FontManager, const FExpression* Expression, uint FontSize, bool IsSmall, EOperationLevel OperationLevel)
		: FUIContainer(FontManager)
		, Expression(Expression)
		, IsConstant(false)
	{
		auto Main = AddElement<FUIExpression>(Expression, FontSize, IsSmall);
		Main->Locate(0);
		Size = Main->Size;
		CenterLine = Main->CenterLine;
		BracketIndex = Main->BracketIndex;

		switch (Expression->Type)
		{
		case EExpressionType::Add:
		case EExpressionType::Sub:
			break;
		case EExpressionType::Mul:
		case EExpressionType::Div:
		case EExpressionType::Function:
			if (OperationLevel < EOperationLevel::Mul)
				return;
			break;
		case EExpressionType::Pow:
			if (OperationLevel < EOperationLevel::Pow)
				return;
			break;
		default:
			return;
		}

		auto Type = Main->BracketIndex % 3;
		EMathSymbol Symbol;
		if (Type == 0)
			Symbol = EMathSymbol::Parentheses;
		else if (Type == 1)
			Symbol = EMathSymbol::CurlyBracket;
		else
			Symbol = EMathSymbol::SquareBracket;

		auto Left = AddElement<FUISymbol>(Symbol, Main->Size.Y, FontSize, true);
		auto Right = AddElement<FUISymbol>(Symbol, Main->Size.Y, FontSize, false);

		Left->Locate(0);
		Main->Locate(int2(Left->Size.X, Main->CenterLine - Main->Size.Y / 2.0f));
		Right->Locate(int2(Left->Size.X + Main->Size.X, 0));
		Size = int2(Left->Size.X + Main->Size.X + Right->Size.X, Left->Size.Y);
		CenterLine = Left->CenterLine;
		BracketIndex++;
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
			auto Element = AddElement<FUIExpression>(Expression, FontSize, false);

			if (IsFirst)
			{
				Offset = int2(0, Element->CenterLine);
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

			auto After = AddElement<FUIExpression>(&Rules[i]->GetBefore(), FontSize, false);
			auto Before = AddElement<FUIExpression>(&Rules[i]->GetAfter(), FontSize, false);
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