#include "UIWindow.h"
#include <Windows.h>
#include <assert.h>

FUIWindow::FUIWindow(FUIApplication* InApplication, unique_ptr<FUIControl> InContent)
	: Application(InApplication)
	, Content(move(InContent))
	, Viewport(nullptr)
	, WindowHandle(nullptr)
	, Title(L"Window")
	, StartupState(EWindowState::Normal)
	, IsTrakingMouseLeave(false)
{
	RECT DesktopRect;
	GetWindowRect(GetDesktopWindow(), &DesktopRect);

	StartupSize = int2
	(
		DesktopRect.right / 1.5f,
		DesktopRect.bottom / 1.5f
	);

	StartupPosition = int2
	(
		DesktopRect.right / 2.0f - StartupSize.X / 2.0f,
		DesktopRect.bottom / 2.0f - StartupSize.Y / 2.0f
	);

	WindowHandle = CreateWindow
	(
		&Application->GetApplicationName()[0],
		&Title[0],
		WS_POPUP,
		StartupPosition.X,
		StartupPosition.Y,
		StartupSize.X,
		StartupSize.Y,
		nullptr,
		nullptr,
		Application->GetInstance(),
		nullptr
	);

	Viewport = Application->GetRenderingManager()->CreateViewport(WindowHandle, StartupSize);
	ShowWindow(WindowHandle, SW_SHOW);	
	Content->Initialize(Application);
	Content->SetWindow(this);
	Content->SetArea(intrect(0, Viewport->GetSize()));

	Viewport->RenderingStart();
	Content->Update();
	Viewport->RenderingEnd();
}

void FUIWindow::ReceiveMessage(SUIWinAPIMessage const& Message)
{
	GetViewport()->RenderingStart();
	GetContent()->ReceiveMessage(Message);
	GetViewport()->RenderingEnd();

	switch (Message.Message)
	{
	case WM_MOUSEMOVE:
		if (!IsTrakingMouseLeave)
		{
			TRACKMOUSEEVENT Event;
			Event.cbSize = sizeof(TRACKMOUSEEVENT);
			Event.dwFlags = TME_LEAVE;
			Event.hwndTrack = WindowHandle;
			Event.dwHoverTime = HOVER_DEFAULT;
			TrackMouseEvent(&Event);
			IsTrakingMouseLeave = true;
		}
		break;
	case WM_MOUSELEAVE:
		IsTrakingMouseLeave = false;
		break;
	case WM_LBUTTONDOWN:
		SetCapture(WindowHandle);
		break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		break;
	}
	
}