#pragma once

enum ERHITextureType
{
	Drawonly, Writeable, RenderTarget, DepthStencil
};

class FRHITexture
{
private:
	int2 Size;
	ERHITextureType Type;

public:
	virtual ~FRHITexture() {}
	virtual void* Map(uint* OutRowSize = nullptr) = 0;
	virtual void Unmap() = 0;
	virtual void ClearRenderTarget(float3 Color) = 0;
	virtual void ClearDepthStencil(float Depth = 0, char Stencil = 0) = 0;

	FRHITexture(ERHITextureType Type, int2 Size) : Type(Type), Size(Size) {}
	int2 GetSize() { return Size; }
	ERHITextureType GetTextureType() { return Type; }
};

class FRHIVertexBuffer
{
private:
	uint VertexStride;
	uint VertexCount;

public:	
	virtual ~FRHIVertexBuffer() {}

	FRHIVertexBuffer(uint VertexStride, uint VertexCount) : VertexStride(VertexStride), VertexCount(VertexCount) {}
	uint GetVertexStride() { return VertexStride; }
	uint GetVertexCount() { return VertexCount; }
};

class FRHIIndexBuffer
{
private:
	uint IndexStride;
	uint IndexCount;

public:
	virtual ~FRHIIndexBuffer() {}

	FRHIIndexBuffer(uint IndexCount, uint IndexStride) : IndexCount(IndexCount), IndexStride(IndexStride){}
	uint GetIndexStride() { return IndexStride; }
	uint GetIndexCount() { return IndexCount; }
};

enum class ERHIShaderType
{
	Vertex, Pixel
};

class FRHIShader
{
private:
	ERHIShaderType ShaderType;

public:
	FRHIShader(ERHIShaderType ShaderType) : ShaderType(ShaderType) {}	
	ERHIShaderType GetShaderType() const { return ShaderType; }
	
	virtual ~FRHIShader() {}
	virtual void SetBuffer(uint Index, const void* Buffer) = 0;
	virtual void SetTexture(uint Index, FRHITexture* Texture) = 0;
};

class FRHIViewport
{
protected:
	int2 Size;

public:
	virtual ~FRHIViewport() {};
	virtual void RenderingStart() = 0;
	virtual void RenderingEnd() = 0;
	virtual void Clear(float3 Color) = 0;
	virtual void Resize(int2 Size) = 0;

	FRHIViewport(int2 Size) : Size(Size) {}
	int2 GetSize() { return Size; }
};