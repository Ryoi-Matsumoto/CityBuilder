#pragma once
#include "Common.h"

class FDX11Viewport : public FRHIViewport
{
private:
	IDXGISwapChain* SwapChain;
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11Device* Device;
	ID3D11DeviceContext* Context;

public:
	~FDX11Viewport() override;
	void RenderingStart() override;	
	void RenderingEnd() override;
	void Clear(float3 Color) override;
	void Resize(int2 Size) override;	
	
	FDX11Viewport(ID3D11Device* Device, DXGI_SAMPLE_DESC SampleDesc, void* WindowHandle, int2 Size, uint RefreshRate);

private:
	void InitializeRenderTargetView();
};