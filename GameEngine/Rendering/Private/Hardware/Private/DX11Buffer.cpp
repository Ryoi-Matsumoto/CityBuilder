#include "DX11Buffer.h"

FDX11VertexBuffer::FDX11VertexBuffer(ID3D11Device* Device, void* Vertexes, uint VertexStride, uint VertexCount)
	: FRHIVertexBuffer(VertexStride, VertexCount)
{
	D3D11_BUFFER_DESC Desc;
	Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Desc.ByteWidth = VertexStride * VertexCount;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA SourceData;
	SourceData.pSysMem = Vertexes;

	HR(Device->CreateBuffer(&Desc, &SourceData, &Buffer));
}

FDX11VertexBuffer::~FDX11VertexBuffer()
{
	Buffer->Release();
}

FDX11IndexBuffer::FDX11IndexBuffer(ID3D11Device* Device, uint* Indexes, uint IndexCount)
	: FRHIIndexBuffer(IndexCount, sizeof(uint))
{
	D3D11_BUFFER_DESC Desc;
	Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Desc.ByteWidth = GetIndexStride() * IndexCount;
	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA SourceData;
	SourceData.pSysMem = Indexes;

	HR(Device->CreateBuffer(&Desc, &SourceData, &Buffer));
}

FDX11IndexBuffer::~FDX11IndexBuffer()
{
	Buffer->Release();
}