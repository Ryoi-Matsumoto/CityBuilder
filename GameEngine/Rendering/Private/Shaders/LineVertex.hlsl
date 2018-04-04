
float4 Position0;
float4 Position1;

float4 main( int Index : POSITION ) : SV_POSITION
{
	if (Index == 0)return Position0;
	else return Position1;
}