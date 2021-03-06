#include "..\Public\RFontLetter.h"
#include "Hardware\Public\RHIDevice.h"
#include "Hardware\Public\RHIResource.h"
#include "FontGenerator.h"

void WriteFontBitmap(DWORD* Target, uint RowSize, const BYTE* Bitmap, int2 BitmapSize, uint Grad)
{	
	for (int Y = 0; Y < BitmapSize.Y; Y++)
	{
		for (int X = 0; X < BitmapSize.X; X++)
		{
			BYTE Alpha = Bitmap[Y * BitmapSize.X + X] * 255 / Grad;
			Target[Y * (RowSize / 4) + X] = Alpha << 24 | 0x00ffffff;
		}
	}
}

FRFontLetter::FRFontLetter(FRHIDevice* Device, FFontGenerator* FontGenerator, wchar_t Letter)
	: IsBrank(false)
	, Letter(Letter)
	, Texture(nullptr)
{
	uint Size = FontGenerator->GetFontType().Size;	

	if (Letter == L' ')
	{
		IsBrank = true;
		FontSize = int2(Size / 2, Size);
	}
	else if (Letter == L'�@')
	{
		IsBrank = true;
		FontSize = int2(Size, Size);
	}
	else if (Letter == L'\t')
	{
		IsBrank = true;
		FontSize = int2(Size * 2, Size);
	}
	else
	{
		FontGenerator->SetLetter(Letter);

		OriginPoint = FontGenerator->GetOriginPoint();
		BlackBoxSize = FontGenerator->GetFontSize();
		FontSize = FontGenerator->GetFontRegion();
		uint Grad = FontGenerator->GetGrad();
		auto Bitmap = FontGenerator->GetBitmap();

		if (Bitmap)
		{
			TextureSize = int2
			(
				pow(2, (int)log2(BlackBoxSize.X) + 1),
				pow(2, (int)log2(BlackBoxSize.Y) + 1)
			);
			Texture = Device->CreateTexture(ERHITextureType::Writeable, TextureSize);

			uint RowSize;
			auto Data = (DWORD*)Texture->Map(&RowSize);
			ZeroMemory(Data, RowSize * TextureSize.Y);
			WriteFontBitmap(Data, RowSize, &Bitmap[0], BlackBoxSize, Grad);
			Texture->Unmap();
		}
	}
}

void FRFontLetter::Render(FR2DRenderer* Renderer, int2 Position, float4 Color, float2 Scale) const
{
	if (Texture)
	{
		intrect Area(OriginPoint + Position, (TextureSize.ToVector() * Scale).ToInt());
		Renderer->DrawTexture(Texture.get(), Area, Color);
	}
}

FRFontSet::FRFontSet(FRHIDevice* Device, const SRFontType& FontType)
	: FontType(FontType)
	, Device(Device)
{
	FontGenerator = make_unique<FFontGenerator>(FontType);
}

FRFontSet::~FRFontSet()
{
}

uint FRFontSet::GetAscent() const 
{ 
	return FontGenerator->GetAscent(); 
}

uint FRFontSet::GetDescent() const 
{ 
	return FontGenerator->GetDescent();
}

FRFontLetter* FRFontSet::GetFontLetter(wchar_t Letter)
{
	FRFontLetter* FontLetter;
	auto FindItr = FontLetters.find(Letter);
	if (FindItr == FontLetters.end())
	{
		FontLetter = new FRFontLetter(Device, FontGenerator.get(), Letter);
		FontLetters[Letter].reset(FontLetter);		
	}
	else
	{
		FontLetter = FindItr->second.get();
	}
	return FontLetter;
}

FRFontSet* FRFontManager::GetFontSet(SRFontType const& FontType)
{
	auto FontTypeEx = SRFontTypeEx(FontType);
	
	FRFontSet* FontSet;
	auto FindItr = FontSets.find(FontType);
	if (FindItr == FontSets.end())
	{
		FontSet = new FRFontSet(Device, FontType);
		FontSets[FontTypeEx].reset(FontSet);
	}
	else
	{
		FontSet = FindItr->second.get();
	}
	return FontSet;
}

#define CompareAndReturn(A) if (A < Right.A) return true; if (A > Right.A) return false;

bool SRFontTypeEx::operator < (const SRFontTypeEx& Right) const
{
	CompareAndReturn(Size);
	CompareAndReturn(Weight);
	CompareAndReturn(Family);
	CompareAndReturn(Itaric);
	CompareAndReturn(Underline);
	CompareAndReturn(StrikeOut);
	return false;
}