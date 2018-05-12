#pragma once

class FR2DObject
{
public:
	virtual void Render(class FR2DRenderer* Renderer, int2 Position, float4 Color, float2 Scale) const = 0;
	virtual uint GetWidth() const = 0;
	virtual uint GetHeight() const = 0;
};

class FR2DRenderer
{
private:
	class FRResourceManager* ResourceManager;
	class FRHIDevice* Device;
	unique_ptr<class FRHIVertexBuffer> RectangleVertexBuffer;
	unique_ptr<class FRHIVertexBuffer> TriangleVertexBuffer;
	float2 ScreenSize;

public:
	FR2DRenderer(class FRHIDevice* Device, class FRResourceManager* ResourceManager);
	void SetDrawArea(intrect Area);
	void DrawRectangle(intrect Rectangle, float4 Color);
	void DrawTriangle(intrect Rectangle, float4 Color, bool Upside);
	void DrawTexture(class FRHITexture* Texture, intrect Rectangle, float4 Color = float4(1));
};