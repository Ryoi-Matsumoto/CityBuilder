#include "DX11Device.h"
#include "DX11Buffer.h"
#include "DX11Shader.h"
#include "DX11Viewport.h"
#include "DX11Texture.h"

inline D3D11_BLEND_DESC GetDefaultBlendDesc()
{
	D3D11_BLEND_DESC Result;
	ZeroMemory(&Result, sizeof(D3D11_BLEND_DESC));
	Result.AlphaToCoverageEnable = FALSE;
	Result.IndependentBlendEnable = FALSE;
	Result.RenderTarget[0].BlendEnable = TRUE;
	Result.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	Result.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	Result.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	Result.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	Result.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	Result.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	Result.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	return Result;
}

inline D3D11_RASTERIZER_DESC GetDefaultRasterizerDesc()
{
	D3D11_RASTERIZER_DESC Result;
	ZeroMemory(&Result, sizeof(D3D11_RASTERIZER_DESC));
	Result.CullMode = D3D11_CULL_BACK;
	Result.FillMode = D3D11_FILL_SOLID;
	Result.DepthClipEnable = TRUE;
	return Result;
}

FDX11Device::FDX11Device()
	: IsReadyDrawing(false)
{
	uint CreateDeviceFlags = 0;
#ifdef _DEBUG
	CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL FeatureLevel;

	HR(D3D11CreateDevice
	(
		nullptr,					// プライマリ・ディスプレイ・アダプタ
		D3D_DRIVER_TYPE_HARDWARE,	// 3Dハードウェアアクセラレーション
		0,							// ソフトウェアドライバは不要
		CreateDeviceFlags,			// デバッグフラグ
		nullptr,					// 最大のフィーチャレベルを選択
		0,							// 上記引数の配列の数
		D3D11_SDK_VERSION,			// SDKバージョン
		&Device,					// 作成されたデバイス
		&FeatureLevel,				// 対応する最大のフィーチャーレベル
		&Context					// 作成されたデバイスコンテキスト
	));

	if (FeatureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		FDebugger::OutputError(L"This computer does not correspond DirectX11");
	}

	HR(Device->CheckMultisampleQualityLevels
	(
		RenderTargetFormat,	// テクスチャー フォーマット
		4,					// マルチサンプリング時のサンプル数
		&MsaaQuality		// 対応可能な品質レベル
	));
	assert(MsaaQuality > 0);

	if (false)
	{
		SampleDesc.Count = 4;
		SampleDesc.Quality = MsaaQuality - 1;
	}
	else
	{
		SampleDesc.Count = 1;
		SampleDesc.Quality = 0;
	}
	
	auto BlendDesc = GetDefaultBlendDesc();
	float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Device->CreateBlendState(&BlendDesc, &BlendState);
	Context->OMSetBlendState(BlendState, BlendFactor, 0xffffffff);

	auto RSDesc = GetDefaultRasterizerDesc();
	HR(Device->CreateRasterizerState(&RSDesc, &RasterizerState));
	Context->RSSetState(RasterizerState);
}

FDX11Device::~FDX11Device()
{
	Device->Release();
	Context->Release();
	BlendState->Release();
	RasterizerState->Release();
}

void FDX11Device::SetDrawArea(intrect Area)
{
	IsReadyDrawing = true;

	D3D11_VIEWPORT Viewport = {0};
	Viewport.TopLeftX = Area.Position.X;
	Viewport.TopLeftY = Area.Position.Y;
	Viewport.Width = Area.Size.X;
	Viewport.Height = Area.Size.Y;
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;

	Context->RSSetViewports(1, &Viewport);
}

inline void SetVertexBuffer(ID3D11DeviceContext* Context, FRHIVertexBuffer* Buffer)
{
	auto VertexBuffer = ((FDX11VertexBuffer*)Buffer)->GetBuffer();
	uint Stride = Buffer->GetVertexStride();
	uint Offset = 0;
	Context->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
}

inline void SetIndexBuffer(ID3D11DeviceContext* Context, FRHIIndexBuffer* Buffer)
{
	auto IndexBuffer = ((FDX11VertexBuffer*)Buffer)->GetBuffer();
	Context->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

inline void CheckDeviceContext(ID3D11DeviceContext* Context)
{
	ID3D11InputLayout* InputLayout = nullptr;
	Context->IAGetInputLayout(&InputLayout);
	assert(InputLayout);

	ID3D11VertexShader* VertexShader = nullptr;
	Context->VSGetShader(&VertexShader, nullptr, 0);
	assert(VertexShader);

	ID3D11PixelShader* PixelShader = nullptr;
	Context->PSGetShader(&PixelShader, nullptr, 0);
	assert(PixelShader);

	ID3D11Buffer* VertexConstantBuffers[]{ nullptr };
	Context->VSGetConstantBuffers(0, 1, VertexConstantBuffers);
	assert(VertexConstantBuffers[0]);

	ID3D11Buffer* PixelConstantBuffers[]{ nullptr };
	Context->PSGetConstantBuffers(0, 1, PixelConstantBuffers);
	assert(PixelConstantBuffers[0]);

	ID3D11Buffer* VertexBuffers[]{ nullptr };
	Context->IAGetVertexBuffers(0, 1, VertexBuffers, 0, 0);
	assert(VertexBuffers[0]);

	ID3D11RenderTargetView* RenderingTargetViews[]{ nullptr };
	ID3D11DepthStencilView* DepthStencilViews[]{ nullptr };
	Context->OMGetRenderTargets(1, RenderingTargetViews, DepthStencilViews);
	assert(RenderingTargetViews[0]);
}

void FDX11Device::Draw(const SRHIDrawContent& Content)
{
	assert(IsReadyDrawing);

	switch (Content.PrimitiveTopology)
	{
	case ERHIPrimitiveTopology::TriangleList:
		Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		break;
	case ERHIPrimitiveTopology::TriangleStrip:
		Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		break;
	case ERHIPrimitiveTopology::LineList:
		Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	default:
		assert("PrimitiveTopology is wrong");
	}
	
	assert(Content.VertexBuffer);	
	SetVertexBuffer(Context, Content.VertexBuffer);

	assert(Content.VertexShader);
	((FDX11Shader*)Content.VertexShader)->Apply();

	assert(Content.PixelShader);	
	((FDX11Shader*)Content.PixelShader)->Apply();

	if (Content.IndexBuffer)
	{
		SetIndexBuffer(Context, Content.IndexBuffer);
	}

#ifdef _DEBUG
	CheckDeviceContext(Context);
#endif	
	
	if (Content.IndexBuffer)
	{
		Context->DrawIndexed(Content.StartIndexLocation, Content.IndexCount, 0);
	}
	else
	{
		Context->Draw(Content.VertexBuffer->GetVertexCount(), 0);
	}
}

unique_ptr<FRHIVertexBuffer> FDX11Device::CreateVertexBuffer(void* Vertexes, uint VertexStride, uint VertexCount)
{
	return make_unique<FDX11VertexBuffer>(Device, Vertexes, VertexStride, VertexCount);
}

unique_ptr<FRHIIndexBuffer> FDX11Device::CreateIndexBuffer(uint* Indexes, uint IndexCount)
{
	return make_unique<FDX11IndexBuffer>(Device, Indexes, IndexCount);
}

unique_ptr<FRHIShader> FDX11Device::CreateShader(ERHIShaderType ShaderType, binarydata Binary)
{
	return make_unique<FDX11Shader>(Device, ShaderType, move(Binary));
}

unique_ptr<FRHITexture> FDX11Device::CreateTexture(ERHITextureType Type, int2 Size)
{
	return make_unique<FDX11Texture>(Device, SampleDesc, Type, Size);
}

unique_ptr<FRHIViewport> FDX11Device::CreateViewport(void* WindowHandle, int2 Size, uint RefreshRate)
{
	return make_unique<FDX11Viewport>(Device, SampleDesc, WindowHandle , Size, RefreshRate);
}

unique_ptr<FRHIDevice> CreateRHIDecice()
{
	return make_unique<FDX11Device>();
}