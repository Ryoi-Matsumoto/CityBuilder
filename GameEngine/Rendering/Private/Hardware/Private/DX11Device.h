#pragma once
#include "Common.h"

class FDX11Device : public FRHIDevice
{
private:
	ID3D11Device* Device;
	ID3D11DeviceContext* Context;
	ID3D11RasterizerState* RasterizerState;
	ID3D11BlendState* BlendState;
	DXGI_SAMPLE_DESC SampleDesc;
	uint MsaaQuality;
	bool IsReadyDrawing;

public:
	FDX11Device();
	~FDX11Device();
	void SetDrawArea(intrect Area) override;
	void Draw(const SRHIDrawContent& Context) override;
	unique_ptr<FRHIVertexBuffer> CreateVertexBuffer(void* Vertexes, uint VertexStride, uint VertexCount) override;
	unique_ptr<FRHIIndexBuffer> CreateIndexBuffer(uint* Indexes, uint IndexCount) override;
	unique_ptr<FRHIShader> CreateShader(ERHIShaderType ShaderType, binarydata Binary) override;
	unique_ptr<FRHITexture> CreateTexture(ERHITextureType Type, int2 Size) override;
	unique_ptr<FRHIViewport> CreateViewport(void* WindowHandle, int2 Size, uint RefreshRate) override;
};