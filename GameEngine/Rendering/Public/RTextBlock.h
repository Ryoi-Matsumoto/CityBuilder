#pragma once
#include "RTextContainer.h"
#include "R2DRenderer.h"

class FRTextBlock : public FRTextContainer, public FR2DObject
{
protected:
	float4 FontColor;
	uint EndIndex;

public:
	FRTextBlock(FRFontSet* FontSet);
	void SetRenderingEndLine(uint InEndIndex) { EndIndex = InEndIndex; }
	void Render(FR2DRenderer* Renderer, int2 Position, float4 Color) const override;
	uint GetWidth() const override;
	uint GetHeight() const override;
};