#include "Grid.h"

FGrid::FGrid(float Size, uint SpritNum, float4 Color):Color(Color)
{
	vector<float3> Vertices;
	float3 Offset(-Size / 2.0f, 0, -Size / 2.0f);
	float Square = Size / SpritNum;
	for (int i = 0; i <= SpritNum; i++)
	{
		Vertices.push_back(Offset + float3(Square * i, 0, 0));
		Vertices.push_back(Offset + float3(Square * i, 0, Square * SpritNum));
	}
	for (int i = 0; i <= SpritNum; i++)
	{
		Vertices.push_back(Offset + float3(0, 0, Square * i));
		Vertices.push_back(Offset + float3(Square * SpritNum, 0, Square * i));
	}

	VertexBuffer = RDevice->CreateVertexBuffer(&Vertices[0], sizeof(float3), Vertices.size());
}

void FGrid::Render()
{	
	VertexBuffer->Apply();
	RShader(SDefaultVertex)->Apply(WVPMatrix);
	RShader(SDefaultPixel)->Apply(Color);
	RDevice->SetPrimitiveTopology(EPrimitiveTopology::LineList);
	RDevice->Draw();
}