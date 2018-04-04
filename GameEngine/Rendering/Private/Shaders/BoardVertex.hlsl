#include "Common.h"
#include "BoardVertex.h"

void main(float3 Position : POSITION, float2 TexCoord : TEXCOORD0, 	
		  out float4 OutPosition : SV_POSITION, out float2 OutTexCoord : TEXCOORD0)
{
	OutPosition = float4(Position, 1.0f) * Scale + Translate;
	OutPosition.x = (OutPosition.x * 4.0f - 2.0f);
	OutPosition.y = -(OutPosition.y * 4.0f - 2.0f);
	OutTexCoord = TexCoord;
}