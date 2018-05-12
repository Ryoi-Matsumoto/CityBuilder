#pragma once
#include "Rendering\Public\RManager.h"
#include <Windows.h>

class FUIWindow;
class FUIControl;

enum class EUIColorType
{
	None, Base, Selection, TabOnCursor, TabButtonOnCursor, Font, Button, ButtonOnCursor, ButtonOnClick, ScrollBack, Scroll, ScrollOnMouse, ScrollOnClick, LineNumber
};

class FUIWindowHolder
{
public:
	virtual FUIWindow* GetWindow() = 0;
};

class FUIApplication
{
private:	
	HINSTANCE InstanceHandle;
	unique_ptr<FRManager> RenderingManager;
	FRFontSet* DefaultFontSet;
	wstring ApplicationName;	

public:
	FUIApplication(HINSTANCE InstanceHandle, wstring const& ApplicationName);
	bool Update();
	FUIWindow* CreateUIWindow(FUIControl* Content, struct SUIWindowStartupState StartupState);
	FRManager* GetRenderingManager() const { return RenderingManager.get(); }	
	FRFontSet* GetDefaultFontSet() const { return DefaultFontSet; }
	HINSTANCE__* GetInstance() const { return InstanceHandle; }
	wstring const& GetApplicationName() const { return ApplicationName; }
	float4 GetColor(EUIColorType Type) const;
};