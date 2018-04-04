#include "..\Public\R2DRenderer.h"
#include "Hardware\Public\RHIDevice.h"
#include "Hardware\Public\RHIResource.h"
#include "RResourceManager.h"
#include "RShader.h"

FR2DRenderer::FR2DRenderer(FRHIDevice* Device, FRResourceManager* ResourceManager)
	: Device(Device)
	, ResourceManager(ResourceManager)
{
	float Vertices[4][5] =
	{
		{ 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f, 1.0f, 0.0f }
	};

	VertexBuffer = Device->CreateVertexBuffer(Vertices, sizeof(float) * 5, 4);
}

void FR2DRenderer::SetDrawArea(intrect Area)
{
	ScreenSize = Area.Size.ToVector();
	Device->SetDrawArea(Area);
}

auto GetBufferBoardVertex(intrect Rectangle, float2 ScreenSize)
{
	FRResourceManager::SRBufferBoardVertex Result;
	Result.Translate = float4(Rectangle.Position.ToVector() / ScreenSize, 0.0f, 1.0f);
	Result.Scale = float4(Rectangle.Size.ToVector() / ScreenSize, 1.0f, 1.0f);
	return Result;
}

void FR2DRenderer::DrawTexture(FRHITexture* Texture, intrect Rectangle, float4 Color)
{
	auto VSBuffer = GetBufferBoardVertex(Rectangle, ScreenSize);

	FRResourceManager::SRBufferBoardTexturePixel PSBuffer;
	PSBuffer.Color = Color;
	PSBuffer.Textures.Texture = Texture;

	SRHIDrawContent Content;
	Content.PrimitiveTopology = ERHIPrimitiveTopology::TriangleStrip;
	Content.VertexBuffer = VertexBuffer.get();
	Content.VertexShader = ResourceManager->GetShaderBoardVertex(VSBuffer);
	Content.PixelShader = ResourceManager->GetShaderBoardTexturePixel(PSBuffer);
	Device->Draw(Content);
}

void FR2DRenderer::DrawRectangle(intrect Rectangle, float4 Color)
{
	auto VSBuffer = GetBufferBoardVertex(Rectangle, ScreenSize);

	FRResourceManager::SRBufferBoardPixel PSBuffer;
	PSBuffer.Color = Color;

	SRHIDrawContent Content;
	Content.PrimitiveTopology = ERHIPrimitiveTopology::TriangleStrip;
	Content.VertexBuffer = VertexBuffer.get();
	Content.VertexShader = ResourceManager->GetShaderBoardVertex(VSBuffer);
	Content.PixelShader = ResourceManager->GetShaderBoardPixel(PSBuffer);
	Device->Draw(Content);
}