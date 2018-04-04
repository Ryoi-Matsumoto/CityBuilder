#include "..\Public\RManager.h"
#include "Hardware\Public\RHIDevice.h"
#include "Hardware\Public\RHIResource.h"
#include "RResourceManager.h"

FRBaseManager::FRBaseManager()
	: Device(CreateRHIDecice())
	, ResourceManager(make_unique<FRResourceManager>(Device.get()))
{
}

FRBaseManager::~FRBaseManager()
{
}

FRManager::FRManager()
	: FR2DRenderer(GetDevice(), GetResourceManager())
	, FRFontManager(GetDevice())	
{
}

FRManager::~FRManager()
{
}

unique_ptr<FRViewport> FRManager::CreateViewport(void* WindowHandle, int2 Size)
{
	return make_unique<FRViewport>(GetDevice(), WindowHandle, Size);
}