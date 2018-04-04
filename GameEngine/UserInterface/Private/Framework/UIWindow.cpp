#include "..\..\Public\Framework\UIWindow.h"
#include <Windows.h>
#include <assert.h>

FUIWindow::FUIWindow(FUIApplication* InApplication, unique_ptr<FUIControl> InContent)
	: Application(InApplication)
	, Content(move(InContent))
	, Viewport(nullptr)
	, WindowHandle(nullptr)
	, _Title(L"Window")
	, _StartupState(EWindowState::Normal)
{
	RECT DesktopRect;
	GetWindowRect(GetDesktopWindow(), &DesktopRect);

	_StartupSize = int2
	(
		DesktopRect.right / 1.5f,
		DesktopRect.bottom / 1.5f
	);

	_StartupPosition = int2
	(
		DesktopRect.right / 2.0f - _StartupSize.X / 2.0f,
		DesktopRect.bottom / 2.0f - _StartupSize.Y / 2.0f
	);

	WindowHandle = CreateWindow
	(
		&Application->GetApplicationName()[0],
		&_Title[0],
		WS_POPUP,
		_StartupPosition.X,
		_StartupPosition.Y,
		_StartupSize.X,
		_StartupSize.Y,
		nullptr,
		nullptr,
		Application->GetInstance(),
		nullptr
	);

	Viewport = Application->GetRenderingManager()->CreateViewport(WindowHandle, _StartupSize);
	ShowWindow(WindowHandle, SW_SHOW);		
	Content->Initialize(Application);
	Content->SetWindow(this);	
	Content->SetArea(intrect(0, Viewport->GetSize()));
	Content->Update();
}