#pragma once

class FRHIDevice;
class FRHIShader;
class FRHITexture;
class FRHIVertexBuffer;
class FRResourceManager;
class FR2DRenderer;

class FR2DObject
{
public:
	virtual void Render(FR2DRenderer* Renderer, int2 Position, float4 Color) const = 0;
	virtual uint GetWidth() const = 0;
	virtual uint GetHeight() const = 0;
};

class FR2DRenderer
{
private:
	FRResourceManager* ResourceManager;
	FRHIDevice* Device;
	unique_ptr<FRHIVertexBuffer> VertexBuffer;
	float2 ScreenSize;

public:
	FR2DRenderer(FRHIDevice* Device, FRResourceManager* ResourceManager);
	void SetDrawArea(intrect Area);
	void DrawRectangle(intrect Rectangle, float4 Color);
	void DrawTexture(FRHITexture* Texture, intrect Rectangle, float4 Color = float4(1));
};