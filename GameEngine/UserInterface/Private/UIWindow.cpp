#include "UIWindow.h"
#include "Container\UIGrid.h"
#include "Complex\UIWindowHeader.h"
#include <Windows.h>
#include <assert.h>

int GetWindowStateCommand(EUIWindowState State)
{
	switch (State)
	{
	case EUIWindowState::Normal:
		return SW_NORMAL;
	case EUIWindowState::Minimized:
		return SW_MINIMIZE;
	case EUIWindowState::Maximized:
		return SW_MAXIMIZE;
	case EUIWindowState::Hide:
		return SW_HIDE;
	default:
		assert(false);
		return SW_NORMAL;
	}
}

void SUIWindowStartupState::Default()
{
	RECT DesktopRect;
	GetWindowRect(GetDesktopWindow(), &DesktopRect);

	Area.Size = int2
	(
		DesktopRect.right / 1.5f,
		DesktopRect.bottom / 1.5f
	);

	Area.Position = int2
	(
		DesktopRect.right / 2.0f - Area.Size.X / 2.0f,
		DesktopRect.bottom / 2.0f - Area.Size.Y / 2.0f
	);
}

FUIWindow::FUIWindow(FUIApplication* InApplication, FUIControl* InContent, SUIWindowStartupState StartupState)
	: Application(InApplication)
	, Viewport(nullptr)
	, WindowHandle(nullptr)
	, IsTrakingMouseLeave(false)
{
	WindowHandle = CreateWindow
	(
		Application->GetApplicationName().data(),
		StartupState.Title.data(),
		WS_POPUP, //| WS_SIZEBOX,
		StartupState.Area.Position.X,
		StartupState.Area.Position.Y,
		StartupState.Area.Size.X,
		StartupState.Area.Size.Y,
		nullptr,
		nullptr,
		Application->GetInstance(),
		nullptr
	);

	RECT DesktopRect;
	GetWindowRect(GetDesktopWindow(), &DesktopRect);

	Viewport = Application->GetRenderingManager()->CreateViewport(WindowHandle, StartupState.Area.Size);
	ShowWindow(WindowHandle, SW_SHOW);

	auto Frame = new FUIControl();
	Frame->SetMargin(SUIMargin(2));
	Frame->SetBackColor(EUIColorType::LineNumber);
	{
		auto Grid = new FUIGrid();
		Grid->AddYDefine(SUILenght());
		Grid->AddYDefine(SUILenght(1, true));
		Grid->SetMargin(SUIMargin(10));
		{
			auto Header = new FUIWindowHeader();
			Header->SetYIndex(0);
			InContent->SetYIndex(1);

			Grid->AddChild(Header);
			Grid->AddChild(InContent);
		}

		Frame->AddChild(Grid);
	}

	Content = unique_ptr<FUIControl>(Frame);
	Content->SetWindow(this);
	Content->Initialize(Application);
	Content->SetArea(intrect(0, Viewport->GetSize()));

	Viewport->RenderingStart();
	Content->Update();
	Viewport->RenderingEnd();
}

LRESULT FUIWindow::ReceiveMessage(SUIWinAPIMessage const& Message)
{
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
	case WM_SIZE:
		Viewport->Resize(GetWindowArea().Size);
		break;
	}

	Viewport->RenderingStart();
	auto Result = GetContent()->ReceiveMessage(Message);
	Viewport->RenderingEnd();

	return Result;
}

intrect FUIWindow::GetWindowArea() const
{
	RECT Rect;
	GetWindowRect(WindowHandle, &Rect);
	int2 Position(Rect.left, Rect.top);
	int2 Size(Rect.right - Rect.left, Rect.bottom - Rect.top);
	return intrect(Position, Size);
}

void FUIWindow::SetWindowState(EUIWindowState State) const
{
	ShowWindow(WindowHandle, GetWindowStateCommand(State));
}