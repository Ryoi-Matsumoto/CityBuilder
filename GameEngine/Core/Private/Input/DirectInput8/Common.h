#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "..\Interface.h"
#include "Core\System\BasicLibrary.h"
#include "Core\System\Math.h"

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

// ƒGƒ‰[ˆ—
#define HR(hr) if (FAILED(hr))throw (hr);