#include "FontGenerator.h"

FFontGenerator::FFontGenerator(const SRFontType& FontType)
	: FontType(FontType)
{
	// windows scores the fonts by weighting the match of each font setting element,
	// and selects the font with the highest score.
	// for exsample, if the specified name font does not match the specified charset or pitch,
	// windows selects the substitute font.
	// therefore, when you specify a font, you have to check the charset or pitch exists in the font
	// with using notepad's font setting wizard and so on.
	HDeviceContext = GetDC(nullptr);
	LOGFONTA Desc = { 0 };
	strcpy(Desc.lfFaceName, FontType.Family.data());   // 10000 points
	Desc.lfCharSet = ANSI_CHARSET;	           // 65000 points
	Desc.lfOutPrecision = OUT_DEFAULT_PRECIS;	   // 19000 points
	Desc.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; // FixedPitch:15000, Family:9000 points
	Desc.lfHeight = FontType.Size;
	Desc.lfWidth = 0;          // average width
	Desc.lfEscapement = 0;     // character feed angle
	Desc.lfOrientation = 0;    // baseline angle
	Desc.lfItalic = FontType.Itaric;
	Desc.lfUnderline = FontType.Underline;
	Desc.lfStrikeOut = FontType.StrikeOut;
	Desc.lfWeight = (LONG)FontType.Weight;
	Desc.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	Desc.lfQuality = PROOF_QUALITY;
	HFont = CreateFontIndirectA(&Desc);
	SelectObject(HDeviceContext, HFont);
	GetTextMetricsW(HDeviceContext, &TextMetric);
}

FFontGenerator::~FFontGenerator()
{
	DeleteObject(HFont);
	ReleaseDC(nullptr, HDeviceContext);
}

void FFontGenerator::SetLetter(wchar_t Letter)
{
	SelectObject(HDeviceContext, HFont);
	int GradFlag = GGO_GRAY8_BITMAP;
	CONST MAT2 Matrix = { { 0,1 },{ 0,0 },{ 0,0 },{ 0,1 } };
	uint Size = GetGlyphOutlineW(HDeviceContext, (uint)Letter, GradFlag, &GlyphMetrics, 0, 0, &Matrix);
	Bitmap.resize(Size);
	if (Size > 0)
	{		
		GetGlyphOutlineW(HDeviceContext, (uint)Letter, GradFlag, &GlyphMetrics, Size, Bitmap.data(), &Matrix);

		Grad = 16;
		switch (GradFlag)
		{
		case GGO_BITMAP: Grad = 1;
		case GGO_GRAY2_BITMAP: Grad = 4; break;
		case GGO_GRAY4_BITMAP: Grad = 16; break;
		case GGO_GRAY8_BITMAP: Grad = 64; break;
		}
	}
}

int2 FFontGenerator::GetOriginPoint() const
{
	return int2
	(
		GlyphMetrics.gmptGlyphOrigin.x,
		TextMetric.tmHeight - GlyphMetrics.gmptGlyphOrigin.y
	);
}

int2 FFontGenerator::GetFontSize() const
{
	return int2
	(
		(GlyphMetrics.gmBlackBoxX + 3) / 4 * 4,
		GlyphMetrics.gmBlackBoxY
	);
}

int2 FFontGenerator::GetFontRegion() const
{
	return int2
	(
		GlyphMetrics.gmCellIncX,
		TextMetric.tmHeight
	);
}

const BYTE* FFontGenerator::GetBitmap() const 
{ 
	if (Bitmap.size() == 0)return nullptr;
	return Bitmap.data();
}