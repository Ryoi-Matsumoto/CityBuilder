#pragma once

#include <d3d11.h>
#pragma comment( lib, "d3d11.lib" )

#include "..\Public\RHIDevice.h"
#include "..\Public\RHIResource.h"

#define HR(X) if(FAILED(X))assert(!X)

constexpr DXGI_FORMAT RenderTargetFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr DXGI_FORMAT DepthSentcilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
/*
template<typename T>
struct SReleaseDeleter
{
	void operator(T* Object)
	{
		Object->Release();
	}
};

template<typename T>
using dxunique_ptr = unique_ptr<T, SReleaseDeleter<T>>;
*/