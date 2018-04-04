#include "Common.h"
#include "BoardPixel.h"

float4 main(float4 Position : SV_POSITION, float2 TexCoord : TEXCOORD) : SV_Target
{
	return Color;
}