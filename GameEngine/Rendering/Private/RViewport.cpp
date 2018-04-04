#include "..\Public\RViewport.h"
#include "..\Public\RManager.h"
#include "Hardware\Public\RHIDevice.h"
#include "Hardware\Public\RHIResource.h"

FRViewport::FRViewport(FRHIDevice* Device, void* WindowHandle, int2 Size)
	: Viewport(Device->CreateViewport(WindowHandle, Size, 60))
{
}

FRViewport::~FRViewport()
{
}

void FRViewport::RenderingStart()
{
	Viewport->RenderingStart();
}

void FRViewport::RenderingEnd()
{
	Viewport->RenderingEnd();
}

void FRViewport::Clear(float3 Color)
{
	Viewport->Clear(Color);
}

void FRViewport::Resize(int2 Size)
{
	Viewport->Resize(Size);
}

int2 FRViewport::GetSize() const
{ 
	return Viewport->GetSize();
}