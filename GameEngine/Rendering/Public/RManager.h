#pragma once
#include "R2DRenderer.h"
#include "RFontLetter.h"
#include "RViewport.h"

class FRResourceManager;
class FRHIDevice;
class FRHIShader;
class FRHIVertexBuffer;

class FRBaseManager
{
private:
	unique_ptr<FRHIDevice> Device;
	unique_ptr<FRResourceManager> ResourceManager;

public:
	FRBaseManager();
	~FRBaseManager();
	FRHIDevice* GetDevice() const { return Device.get(); }
	FRResourceManager* GetResourceManager() const { return ResourceManager.get(); }
};

class FRManager : public FRBaseManager, public FR2DRenderer, public FRFontManager
{
public:
	FRManager();
	~FRManager();
	unique_ptr<FRViewport> CreateViewport(void* WindowHandle, int2 Size);
}; 