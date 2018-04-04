#pragma once

class FRHIDevice;
class FRHIViewport;
class FRManager;

class FRViewport
{
private:
	unique_ptr<FRHIViewport> Viewport;

public:
	FRViewport(FRHIDevice* Device, void* WindowHandle, int2 Size);
	~FRViewport();
	void RenderingStart();
	void RenderingEnd();
	void Clear(float3 Color);
	void Resize(int2 Size);
	int2 GetSize() const;
};