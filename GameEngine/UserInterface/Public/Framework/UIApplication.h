#pragma once
#include "Rendering\Public\RManager.h"
#include "UICore.h"

class FUIWindow;
class FUIControl;

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
	unique_ptr<FUIWindow> CreateUIWindow(unique_ptr<FUIControl> Content);
	FRManager* GetRenderingManager() const { return RenderingManager.get(); }	
	FRFontSet* GetDefaultFontSet() const { return DefaultFontSet; }
	HINSTANCE__* GetInstance() const { return InstanceHandle; }
	wstring const& GetApplicationName() const { return ApplicationName; }
};