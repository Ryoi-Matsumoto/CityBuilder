#pragma once
#include "..\Public\RFontLetter.h"
#include <Shlwapi.h>
#include <tchar.h>

struct FFontGenerator
{
private:
	HFONT HFont;
	HFONT HOldFont;
	HDC HDeviceContext;
	TEXTMETRIC TextMetric;
	GLYPHMETRICS GlyphMetrics;
	vector<BYTE> Bitmap;	
	uint Grad;
	SRFontType FontType;

public:
	FFontGenerator(CRRFontType FontType);
	~FFontGenerator();
	void SetLetter(wchar_t Letter);
	
	int2 GetOriginPoint() const;
	int2 GetFontSize() const;
	int2 GetFontRegion() const;
	uint GetGrad() const { return Grad; }
	const BYTE* GetBitmap() const;
	CRRFontType GetFontType() const { return FontType; }
};