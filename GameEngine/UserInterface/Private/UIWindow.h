#pragma once
#include "Rendering\Public\RViewport.h"
#include "UIApplication.h"
#include "UIControl.h"

enum class EUIWindowState
{
	Normal, Minimized, Maximized, Hide
};

struct SUIWindowStartupState
{
	intrect Area;
	EUIWindowState State;
	wstring Title;

	void Default();
};

class FUIWindow
{
private:
	FUIApplication* Application;
	unique_ptr<class FUIControl> Content;
	unique_ptr<FRViewport> Viewport;
	HWND WindowHandle;
	wstring Title;

	bool IsTrakingMouseLeave;
	
public:
	FUIWindow(FUIApplication* Application, class FUIControl* Content, SUIWindowStartupState StartupState);

	LRESULT ReceiveMessage(struct SUIWinAPIMessage const& Message);
	FUIApplication* GetApplication() const { return Application; }
	FRViewport* GetViewport() const { return Viewport.get(); }
	class FUIControl* GetContent() const { return Content.get(); }
	HWND__* GetWindowHandle() const { return WindowHandle; }
	intrect GetWindowArea() const;
	wstring GetTitle() const { return Title; }
	void SetWindowState(EUIWindowState State) const;
};