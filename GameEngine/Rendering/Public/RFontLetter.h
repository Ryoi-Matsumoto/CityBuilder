#pragma once
#include "R2DRenderer.h"

enum class ERFontWeight
{
	DontCare = 0,
	Thin = 100,
	ExtraLight = 200,
	UltraLight = 200,
	Light = 300,
	Normal = 400,
	Regular = 400,
	Medium = 500,
	SemiBold = 600,
	DemiBold = 600,
	Bold = 700,
	ExtraBold = 800,
	UltraBold = 800,
	Heavy = 900
};

struct SRFontType
{
	SRFontType()
		: Size(28)
		, Family("‚l‚r ‚oƒSƒVƒbƒN")
		, Itaric(false)
		, Underline(false)
		, StrikeOut(false)
		, Weight(ERFontWeight::Normal)
	{
	}

	int Size;
	string Family;
	bool Itaric;
	bool Underline;
	bool StrikeOut;
	ERFontWeight Weight;
};

class FRHITexture;
class FFontGenerator;

class FRFontLetter : public FR2DObject
{
private:
	unique_ptr<FRHITexture> Texture;
	Matrix WorldMatrix;
	int2 OriginPoint;
	int2 BlackBoxSize; 
	int2 FontSize;
	int2 TextureSize;
	wchar_t Letter;
	bool IsBrank = false;

public:
	FRFontLetter(class FRHIDevice* Device, FFontGenerator* FontGenerator, wchar_t Letter);

	void Render(FR2DRenderer* Renderer, int2 Position, float4 Color, float2 Scale = 1) const override;
	uint GetWidth() const override { return FontSize.X; }
	uint GetHeight() const override { return FontSize.Y; }
	int2 GetOriginPoint() const { return OriginPoint; }
	int2 GetBlackBoxSize() const { return BlackBoxSize; }
	wchar_t GetLetter() const { return Letter; }
};

class FFontGenerator;

class FRFontSet
{
private:
	class FRHIDevice* Device;
	unique_ptr<FFontGenerator> FontGenerator;
	SRFontType FontType;
	map<wchar_t, unique_ptr<FRFontLetter>> FontLetters;

public:
	FRFontSet(class FRHIDevice* Device, SRFontType const& FontType);
	~FRFontSet();

	uint GetAscent() const;
	uint GetDescent() const;
	FRFontLetter* GetFontLetter(wchar_t Letter);
	SRFontType GetFontType() const { return FontType; }
};

struct SRFontTypeEx : public SRFontType
{
	SRFontTypeEx(SRFontType Base) : SRFontType(Base) {}

	bool operator < (const SRFontTypeEx& Right) const;
};

class FRFontManager
{
private:
	class FRHIDevice* Device;
	map<SRFontTypeEx, unique_ptr<FRFontSet>> FontSets;

public:
	FRFontManager(class FRHIDevice* Device)
		: Device(Device)
	{
	}

	FRFontSet* GetFontSet(const SRFontType& FontType);
};