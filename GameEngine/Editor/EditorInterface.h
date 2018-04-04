#pragma once
#include "Common.h"

class IEditor
{
public:
	virtual void OnStart() = 0;
	virtual void OnEnd() = 0;
	virtual void OnKeyPressed(const char* Keys) = 0;
	virtual void OnMouseMoved(Vector2 MousePosition, bool bLeft, bool bMiddle, bool bRight) = 0;
	virtual void OnRendering() = 0;	
	virtual void SetMode() = 0;
	virtual void SetCamera() = 0;
};