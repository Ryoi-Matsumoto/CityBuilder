#pragma once
#include "Rendering\Public\RViewport.h"
#include "UICore.h"
#include "UIApplication.h"
#include "UIControl.h"

enum class EWindowState
{
	Normal, Minimized, Maximized, Hide
};

class FUIControl;

class FUIWindow
{
private:
	FUIApplication* Application;
	unique_ptr<FUIControl> Content;
	unique_ptr<FRViewport> Viewport;
	HWND WindowHandle;
	
public:
	FUIWindow(FUIApplication* Application, unique_ptr<FUIControl> Content);

	FUIApplication* GetApplication() const { return Application; }
	FRViewport* GetViewport() const { return Viewport.get(); }
	FUIControl* GetContent() const { return Content.get(); }
	HWND__* GetWindowHandle() const { return WindowHandle; }

	UIAttribute(wstring, Title)
	UIAttribute(EWindowState, StartupState)
	UIAttribute(int2, StartupSize)
	UIAttribute(int2, StartupPosition)
};