#pragma once
#include "Hardware\Public\RHIDevice.h"
#include "Hardware\Public\RHIResource.h"
#include "RShader.h"

class FRResourceManager : public FRShaderManager
{
private:
	FRHIDevice* Device;

public:
	FRResourceManager(FRHIDevice* Device);
};