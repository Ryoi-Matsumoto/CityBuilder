#pragma once
#include "Rendering\Public\RViewport.h"
#include "UIApplication.h"
#include "UIControl.h"

enum class EWindowState
{
	Normal, Minimized, Maximized, Hide
};

class FUIWindow
{
private:
	FUIApplication* Application;
	unique_ptr<class FUIControl> Content;
	unique_ptr<FRViewport> Viewport;
	HWND WindowHandle;

	wstring Title;
	EWindowState StartupState;
	int2 StartupSize;
	int2 StartupPosition;
	bool IsTrakingMouseLeave;
	
public:
	FUIWindow(FUIApplication* Application, unique_ptr<class FUIControl> Content);

	void ReceiveMessage(struct SUIWinAPIMessage const& Message);
	FUIApplication* GetApplication() const { return Application; }
	FRViewport* GetViewport() const { return Viewport.get(); }
	class FUIControl* GetContent() const { return Content.get(); }
	HWND__* GetWindowHandle() const { return WindowHandle; }

	void SetTitle(wstring InTitle) { Title = InTitle; }
};