#pragma once
#include "RHIResource.h"

enum class ERHIPrimitiveTopology
{
	TriangleList, TriangleStrip, LineList
};

struct SRHIDrawContent
{
	SRHIDrawContent()
		: PrimitiveTopology(ERHIPrimitiveTopology::TriangleList)
		, VertexBuffer(nullptr)
		, IndexBuffer(nullptr)
		, VertexShader(nullptr)
		, PixelShader(nullptr)
		, StartIndexLocation(0)
		, IndexCount(0)
		, StartVertex(0)
		, VertexCount(0)
	{
	}

	ERHIPrimitiveTopology PrimitiveTopology;
	FRHIVertexBuffer* VertexBuffer;
	FRHIIndexBuffer* IndexBuffer;
	FRHIShader* VertexShader;
	FRHIShader* PixelShader;
	uint StartIndexLocation;
	uint IndexCount;
	uint StartVertex;
	uint VertexCount;
};

class FRHIDevice
{
public:
	virtual ~FRHIDevice() {}
	virtual void SetDrawArea(intrect Area) = 0;
	virtual void Draw(const SRHIDrawContent& Context) = 0;
	virtual unique_ptr<FRHIVertexBuffer> CreateVertexBuffer(void* Vertexes, uint VertexStride, uint VertexCount) = 0;
	virtual unique_ptr<FRHIIndexBuffer> CreateIndexBuffer(uint* Indexes, uint IndexCount) = 0;
	virtual unique_ptr<FRHIShader> CreateShader(ERHIShaderType ShaderType, vector<char> Binary) = 0;
	virtual unique_ptr<FRHITexture> CreateTexture(ERHITextureType Type, int2 Size) = 0;
	virtual unique_ptr<FRHIViewport> CreateViewport(void* WindowHandle, int2 Size, uint RefreshRate) = 0;
};

extern unique_ptr<FRHIDevice> CreateRHIDecice();