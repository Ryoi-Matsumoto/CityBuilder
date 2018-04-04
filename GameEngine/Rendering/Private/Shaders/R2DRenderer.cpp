#include "..\Public\R2DRenderer.h"
#include "Hardware\Public\RHIDevice.h"
#include "Hardware\Public\RHIResource.h"
#include "RResourceManager.h"
#include "RShader.h"

FR2DRenderer::FR2DRenderer(FRHIDevice* Device, FRResourceManager* ResourceManager)
	: Device(Device)
{
	float Vertices[4][5] =
	{
		{ 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
	};

	VertexBuffer = Device->CreateVertexBuffer(Vertices, sizeof(float) * 5, 4);

	FRShaderBoardVertex::Preparate(Device, ResourceManager);
}

FR2DRenderer::~FR2DRenderer()
{
	delete VertexBuffer;
}

void FR2DRenderer::SetDrawArea(intrect Area)
{
	ScreenSize = Area.Size.ToVector();
	Device->SetDrawArea(Area);
}

void GetBufferBoardVertex(SRBufferBoardVertex* Out, intrect Rectangle, float2 ScreenSize)
{
	Out->Translate = float4(Rectangle.Position.ToVector() / ScreenSize, 0.0f, 1.0f);
	Out->Scale = float4(Rectangle.Size.ToVector() / ScreenSize, 1.0f, 1.0f);
}

void FR2DRenderer::DrawTexture(FRHITexture* Texture, intrect Rectangle, float4 Color = float4(1))
{
	SRBufferBoardVertex VSBuffer;
	GetBufferBoardVertex(&VSBuffer, Rectangle, ScreenSize);

	SRBufferBoardTexturePixel PSBuffer;
	PSBuffer.Textures.Texture = Texture;

	SRHIDrawContent Content;
	Content.PrimitiveTopology = ERHIPrimitiveTopology::TriangleStrip;
	Content.VertexBuffer = VertexBuffer;
	Content.VertexShader = FRShaderBoardVertex::GetShader(&VSBuffer);
	Content.PixelShader = FRShaderBoardTexturePixel::GetShader(&PSBuffer);
	Device->Draw(Content);
}

void FR2DRenderer::DrawRectangle(intrect Rectangle, float4 Color)
{
	SRBufferBoardVertex VSBuffer;
	GetBufferBoardVertex(&VSBuffer, Rectangle, ScreenSize);

	SRBufferBoardPixel PSBuffer;
	PSBuffer.Color = Color;

	SRHIDrawContent Content;
	Content.PrimitiveTopology = ERHIPrimitiveTopology::TriangleStrip;
	Content.VertexBuffer = VertexBuffer;
	Content.VertexShader = FRShaderBoardVertex::GetShader(&VSBuffer);
	Content.PixelShader = FRShaderBoardPixel::GetShader(&PSBuffer);
	Device->Draw(Content);
}