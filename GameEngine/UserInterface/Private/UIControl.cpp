#include "UIControl.h"
#include <Windows.h>

FUIControl::FUIControl()
	: XIndex(0)
	, YIndex(0)
	, Margin(0)
	, BackColor(EUIColorType::Base)
	, Parent(nullptr)
{
	for (uint i = 0; i < 3; i++)
		MouseButtonStates[i] = false;
}

void FUIControl::ChangeCursor(LPCWSTR CursorType)
{
	HCURSOR HCursor;
	HCursor = LoadCursor(NULL, CursorType);
	::SetCursor(HCursor);
}

FUIControl* TimerControl = nullptr;

void FUIControl::RegisterTimer(uint Millisecond)
{
	struct Proc
	{	 
		static void CALLBACK TimerProcedure(HWND WindowHandle, UINT Message, UINT_PTR EventId, DWORD SystemTime)
		{
			if (TimerControl)
			{
				TimerControl->GetWindow()->GetViewport()->RenderingStart();
				TimerControl->OnTimer();
				TimerControl->GetWindow()->GetViewport()->RenderingEnd();
			}
		}
	};

	TimerControl = this;
	::SetTimer(Window->GetWindowHandle(), 0, Millisecond, Proc::TimerProcedure);
}

void FUIControl::SetWindow(FUIWindow* InWindow)
{
	Window = InWindow;
	OnWindowChanged();
}

void FUIControl::SetArea(intrect InArea)
{
	Area = InArea;
	OnAreaChanged();
}

void FUIControl::Initialize(FUIApplication* InApplication)
{
	Renderer = InApplication->GetRenderingManager();
	OnInitialize(InApplication);
}

void FUIControl::ReceiveMessage(SUIWinAPIMessage const& Message)
{
	int2 MousePosition = Message.GetInt2() - Area.Position + int2(Margin.Left, Margin.Top);
	
	switch (Message.Message)
	{
	case WM_CHAR:
		OnCharInputed(Message.GetChar());
		break;
	case WM_KEYDOWN:
		OnKeyDown(Message.GetChar());
		break;
	case WM_KEYUP:
		OnKeyUp(Message.GetChar());
		break;
	case WM_LBUTTONDBLCLK:
		OnDoubleClick(MousePosition);
		break;
	case WM_LBUTTONDOWN:
		MouseButtonStates[(int)EMouseButton::Left] = true;
		OnMouseDown(EMouseButton::Left, MousePosition);
		break;
	case WM_RBUTTONDOWN:
		MouseButtonStates[(int)EMouseButton::Right] = true;
		OnMouseDown(EMouseButton::Right, MousePosition);
		break;
	case WM_MBUTTONDOWN:
		MouseButtonStates[(int)EMouseButton::Middle] = true;
		OnMouseDown(EMouseButton::Middle, MousePosition);
		break;
	case WM_LBUTTONUP:
		MouseButtonStates[(int)EMouseButton::Left] = false;
		OnMouseUp(EMouseButton::Left, MousePosition);
		break;
	case WM_RBUTTONUP:
		MouseButtonStates[(int)EMouseButton::Right] = false;
		OnMouseUp(EMouseButton::Right, MousePosition);
		break;
	case WM_MBUTTONUP:
		MouseButtonStates[(int)EMouseButton::Middle] = false;
		OnMouseUp(EMouseButton::Middle, MousePosition);
		break;
	case WM_MOUSELEAVE:
		for (uint i = 0; i < 3; i++)
		{
			if (MouseButtonStates[i])
				OnMouseUp((EMouseButton)i, MousePosition);
		}
		OnMouseLeave(MousePosition);
		break;
	case WM_MOUSEWHEEL:
		OnMouseWheel(Message.GetWheelDelta(), MousePosition);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(MousePosition);
		break;
	case WM_SIZE:
		SetArea(intrect(0, Message.GetInt2()));
		Update();
		switch (Message.WParameter)
		{
		case SIZE_RESTORED:
			break;
		case SIZE_MAXIMIZED:
			break;
		case SIZE_MINIMIZED:
			break;
		case SIZE_MAXSHOW:
			break;
		case SIZE_MAXHIDE:
			break;
		}
	}
}

void FUIControl::Update()
{	
	Renderer->SetDrawArea(Area);
	Renderer->DrawRectangle(intrect(int2(0), Area.Size), GetWindow()->GetApplication()->GetColor(BackColor));
	Renderer->SetDrawArea(GetRenderingArea());
	OnRendering();
}

int2 FUIControl::GetRenderingSize() const
{
	return int2(Area.Size.X - Margin.Left - Margin.Right, Area.Size.Y - Margin.Top - Margin.Bottom);
}

int2 FUIControl::GetRenderingPosition() const
{
	return Area.Position + int2(Margin.Left, Margin.Top);
}