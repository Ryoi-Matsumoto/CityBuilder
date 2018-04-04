#include "Common.h"
#include "BoardTexturePixel.h"

SamplerState SamplerLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4 main(float4 Position : SV_POSITION, float2 TexCoord : TEXCOORD) : SV_Target
{
	return Texture.Sample(SamplerLinear, TexCoord) * Color;
}