#pragma once
#include "Common.h"
#include "DX11Device.h"

class FDX11Buffer
{
protected:
	ID3D11Buffer* Buffer;
	
public:
	ID3D11Buffer* GetBuffer() { return Buffer; }
};

class FDX11VertexBuffer : public FDX11Buffer, public FRHIVertexBuffer
{
public:	
	~FDX11VertexBuffer() override;	

	FDX11VertexBuffer(ID3D11Device* Device, void* Vertexes, uint VertexStride, uint VertexCount);
};

class FDX11IndexBuffer : public FDX11Buffer, public FRHIIndexBuffer
{
public:
	~FDX11IndexBuffer() override;

	FDX11IndexBuffer(ID3D11Device* Device, uint* Indexes, uint IndexCount);	
};