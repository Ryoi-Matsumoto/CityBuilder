#pragma once
#include "Common.h"

class FDX11Texture : public FRHITexture
{
private:
	ID3D11Texture2D* Texture;	
	ID3D11DeviceContext* Context;
	union
	{
		IUnknown* View;
		ID3D11ShaderResourceView* ShaderResourceView;
		ID3D11RenderTargetView* RenderTargetView;
		ID3D11DepthStencilView* DepthStencilView;
	};

public:
	~FDX11Texture() override;
	void* Map(uint* OutRowSize) override;
	void Unmap() override;
	void ClearRenderTarget(float3 Color) override;
	void ClearDepthStencil(float Depth, char Stencil) override;
	
	FDX11Texture(ID3D11Device* Device, DXGI_SAMPLE_DESC SampleDesc, ERHITextureType Type, int2 Size);
	ID3D11ShaderResourceView* GetShaderResourceView() 
	{ 
		assert(GetTextureType() == ERHITextureType::Drawonly 
			|| GetTextureType() == ERHITextureType::Writeable);
		return ShaderResourceView; 
	}
	ID3D11RenderTargetView* GetRenderTargetView()
	{
		assert(GetTextureType() == ERHITextureType::RenderTarget);
		return RenderTargetView;
	}
	ID3D11DepthStencilView* GetDepthStencilView()
	{
		assert(GetTextureType() == ERHITextureType::DepthStencil);
		return DepthStencilView;
	}
	ID3D11Texture2D* GetTexture() { return Texture; }
};