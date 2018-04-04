
float4x4 Matrix;

float4 main(float3 Position : POSITION) : SV_POSITION
{
	return mul(float4(Position, 1.0f), Matrix);
}