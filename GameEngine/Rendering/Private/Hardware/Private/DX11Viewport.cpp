#include "DX11Viewport.h"
#include "DX11Device.h"

FDX11Viewport::FDX11Viewport(ID3D11Device* Device, DXGI_SAMPLE_DESC SampleDesc, void* WindowHandle, int2 Size, uint RefreshRate)
	: FRHIViewport(Size)
	, Device(Device)
{
	DXGI_SWAP_CHAIN_DESC Desc;
	Desc.BufferDesc.Width = Size.X;
	Desc.BufferDesc.Height = Size.Y;
	Desc.BufferDesc.RefreshRate.Numerator = RefreshRate;
	Desc.BufferDesc.RefreshRate.Denominator = 1;
	Desc.BufferDesc.Format = RenderTargetFormat;
	Desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	Desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	Desc.SampleDesc = SampleDesc;
	Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Desc.BufferCount = 1;
	Desc.OutputWindow = (HWND)WindowHandle;
	Desc.Windowed = true;
	Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	Desc.Flags = 0;

	Device->GetImmediateContext(&Context);

	// スワップチェーンを正しく作成するには、デバイスの作成に使用されたIDXGIFactoryを使用する必要がある。
	// CreateDXGIFactoryを呼び出して別のIDXGIFactoryインスタンスを使用することはできない。
	IDXGIDevice* DxgiDevice;
	HR(Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&DxgiDevice));

	IDXGIAdapter* DxgiAdapter;
	HR(DxgiDevice->GetAdapter(&DxgiAdapter));

	IDXGIFactory* DxgiFactory;
	HR(DxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&DxgiFactory));

	HR(DxgiFactory->CreateSwapChain(Device, &Desc, &SwapChain));
	
	DxgiDevice->Release();
	DxgiAdapter->Release();
	DxgiFactory->Release();

	InitializeRenderTargetView();
}

FDX11Viewport::~FDX11Viewport()
{
	SwapChain->Release();
}

void FDX11Viewport::Resize(int2 InSize)
{
	Size = InSize;

	Context->OMSetRenderTargets(0, nullptr, nullptr);
	//RenderTargetView->Release();
	DXGI_SWAP_CHAIN_DESC Desc;
	SwapChain->GetDesc(&Desc);

	auto ModeDesc = Desc.BufferDesc;
	ModeDesc.Width = Size.X;
	ModeDesc.Height = Size.Y;
	
	//HR(SwapChain->ResizeTarget(&ModeDesc));
	//HR(SwapChain->ResizeBuffers(Desc.BufferCount, Size.X, Size.Y, Desc.BufferDesc.Format, Desc.Flags));

	InitializeRenderTargetView();
}

void FDX11Viewport::InitializeRenderTargetView()
{
	ID3D11Texture2D* BackBuffer;
	HR(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer));	
	HR(Device->CreateRenderTargetView(BackBuffer, 0, &RenderTargetView));
	BackBuffer->Release();
}

void FDX11Viewport::RenderingEnd()
{
	SwapChain->Present(0, 0);
}

void FDX11Viewport::RenderingStart()
{ 
	Context->OMSetRenderTargets(1, &RenderTargetView, nullptr);
}

void FDX11Viewport::Clear(float3 Color)
{
	Context->ClearRenderTargetView(RenderTargetView, float4(Color, 1.0f).V);
}