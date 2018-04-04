#include "FontGenerator.h"

FFontGenerator::FFontGenerator(CRRFontType FontType)
	: FontType(FontType)
{
	LOGFONTA Desc = { 0 };
	memcpy(Desc.lfFaceName, &FontType.Family[0], sizeof(char) * FontType.Family.size());
	HDeviceContext = GetDC(nullptr);
	Desc.lfHeight = FontType.Size;
	Desc.lfWidth = 0;		   // ���ϕ����� 	
	Desc.lfEscapement = 0;     // ������������̊p�x 
	Desc.lfOrientation = 0;    // �x�[�X���C���̊p�x	
	Desc.lfItalic = FontType.Itaric;
	Desc.lfUnderline = FontType.Underline;
	Desc.lfStrikeOut = FontType.StrikeOut;
	Desc.lfWeight = (LONG)FontType.Weight;
	Desc.lfCharSet = SHIFTJIS_CHARSET; // �����Z�b�g�̎��ʎq 
	Desc.lfOutPrecision = OUT_DEFAULT_PRECIS; // �o�͐��x 
	Desc.lfClipPrecision = CLIP_DEFAULT_PRECIS;	// �N���b�s���O���x 
	Desc.lfQuality = PROOF_QUALITY;	// �o�͕i�� 
	Desc.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;  // �s�b�`�ƃt�@�~��  
	HFont = CreateFontIndirectA(&Desc);
	HOldFont = (HFONT)SelectObject(HDeviceContext, HFont);
}

FFontGenerator::~FFontGenerator()
{
	SelectObject(HDeviceContext, HOldFont);
	DeleteObject(HFont);
	ReleaseDC(nullptr, HDeviceContext);
}

void FFontGenerator::SetLetter(wchar_t Letter)
{
	GetTextMetricsW(HDeviceContext, &TextMetric);
	int GradFlag = GGO_GRAY2_BITMAP;
	CONST MAT2 Matrix = { { 0,1 },{ 0,0 },{ 0,0 },{ 0,1 } };
	uint Size = GetGlyphOutlineW(HDeviceContext, (uint)Letter, GradFlag, &GlyphMetrics, 0, 0, &Matrix);
	Bitmap.resize(Size);
	if (Size > 0)
	{		
		GetGlyphOutlineW(HDeviceContext, (uint)Letter, GradFlag, &GlyphMetrics, Size, &Bitmap[0], &Matrix);

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
		-GlyphMetrics.gmptGlyphOrigin.y
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
	return &Bitmap[0]; 
}