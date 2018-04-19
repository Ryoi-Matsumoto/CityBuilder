#include "Common.h"
#include "BoardTexturePixel.h"

SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4 main(float4 Position : SV_POSITION, float2 TexCoord : TEXCOORD) : SV_Target
{
	return Texture.Sample(Sampler, TexCoord) * Color;
}