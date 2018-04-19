#include "DX11Texture.h"
#include "DX11Device.h"

void CreateShaderResourceView(ID3D11ShaderResourceView** Out, ID3D11Device* Device, ID3D11Texture2D* Texture)
{
	D3D11_TEXTURE2D_DESC TexDesc;
	Texture->GetDesc(&TexDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC SrvDesc;
	ZeroMemory(&SrvDesc, sizeof(SrvDesc));
	SrvDesc.Format = TexDesc.Format;
	SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SrvDesc.Texture2D.MostDetailedMip = 0;
	SrvDesc.Texture2D.MipLevels = TexDesc.MipLevels;

	HR(Device->CreateShaderResourceView(Texture, &SrvDesc, Out));
}

FDX11Texture::FDX11Texture(ID3D11Device* Device, DXGI_SAMPLE_DESC SampleDesc, ERHITextureType Type, int2 Size)
	: FRHITexture(Type, Size)
{	
	Device->GetImmediateContext(&Context);

	D3D11_TEXTURE2D_DESC Desc = {0};
	Desc.Width = Size.X;
	Desc.Height = Size.Y;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = RenderTargetFormat;
	Desc.SampleDesc = SampleDesc;
	
	switch (Type)
	{
	case Drawonly:			
		Desc.Format = RenderTargetFormat;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;		
		Desc.CPUAccessFlags = 0;
		break;
	case Writeable:		
		Desc.Format = RenderTargetFormat;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE; 
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	case RenderTarget:
		Desc.Format = RenderTargetFormat;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.BindFlags = D3D11_BIND_RENDER_TARGET;
		Desc.CPUAccessFlags = 0;
		break;
	case DepthStencil:
		Desc.Format = DepthSentcilFormat;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		Desc.CPUAccessFlags = 0;
		break;
	default:
		FDebugger::OutputError(L"Type is wrong.");
		return;
	}

	HR(Device->CreateTexture2D(&Desc, nullptr, &Texture));

	switch (Type)
	{
	case Drawonly:
		CreateShaderResourceView(&ShaderResourceView, Device, Texture);
		break;
	case Writeable:
		CreateShaderResourceView(&ShaderResourceView, Device, Texture);
		break;
	case RenderTarget:
		HR(Device->CreateRenderTargetView(GetTexture(), nullptr, &RenderTargetView));
		break;
	case DepthStencil:
		HR(Device->CreateDepthStencilView(Texture, 0, &DepthStencilView));
		break;
	}	
}

FDX11Texture::~FDX11Texture()
{	
	Texture->Release();
	View->Release();
}

void* FDX11Texture::Map(uint* OutRowSize)
{
	assert(GetTextureType() == ERHITextureType::Writeable);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HR(Context->Map(Texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));

	if (OutRowSize)
	{
		*OutRowSize = MappedResource.RowPitch;
	}

	return MappedResource.pData;
}

void FDX11Texture::Unmap()
{	
	Context->Unmap(Texture, 0);
}

void FDX11Texture::ClearRenderTarget(float3 Color)
{
	assert(GetTextureType() == ERHITextureType::RenderTarget);
	Context->ClearRenderTargetView(RenderTargetView, float4(Color, 1.0f).V);
}

void FDX11Texture::ClearDepthStencil(float Depth, char Stencil)
{
	assert(GetTextureType() == ERHITextureType::DepthStencil);
	const uint Flags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
	Context->ClearDepthStencilView(DepthStencilView, Flags, Depth, Stencil);
}