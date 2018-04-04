#include "..\..\Public\Framework\UIControl.h"
#include "UIWinAPIMessage.h"
#include <Windows.h>

FUIControl::FUIControl()
	: _XIndex(0)
	, _YIndex(0)
	, _Margin(0)
{
}

void FUIControl::ChangeCursor(ECursorType CursorType)
{
	HCURSOR HCursor;
	HCursor = LoadCursor(NULL, IDC_IBEAM);
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
				TimerControl->OnTimer();
			}
		}
	};

	TimerControl = this;
	::SetTimer(Window->GetWindowHandle(), 0, Millisecond, Proc::TimerProcedure);
}

void FUIControl::RegisterTrackMouseLeave()
{
	TRACKMOUSEEVENT Event;
	Event.cbSize = sizeof(TRACKMOUSEEVENT);
	Event.dwFlags = TME_LEAVE;
	Event.hwndTrack = Window->GetWindowHandle();
	Event.dwHoverTime = HOVER_DEFAULT;
	TrackMouseEvent(&Event);
}

void FUIControl::SetWindow(FUIWindow* InWindow)
{
	Window = InWindow;
	OnWindowChanged();
}

void FUIControl::SetArea(intrect InArea)
{
	Area = _Margin.CuttedOut(InArea);
	OnAreaChanged();
}

void FUIControl::Initialize(FUIApplication* InApplication)
{
	Renderer = InApplication->GetRenderingManager();
	OnInitialize(InApplication);
}

void FUIControl::ReceiveMessage(SUIWinAPIMessage const& Message)
{
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
	case WM_LBUTTONDOWN:
		OnMouseDown(EMouseButton::Left, Message.GetInt2());
		break;
	case WM_RBUTTONDOWN:
		OnMouseDown(EMouseButton::Right, Message.GetInt2());
		break;
	case WM_MBUTTONDOWN:
		OnMouseDown(EMouseButton::Middle, Message.GetInt2());
		break;
	case WM_LBUTTONUP:
		OnMouseUp(EMouseButton::Left, Message.GetInt2());
		break;
	case WM_RBUTTONUP:
		OnMouseUp(EMouseButton::Right, Message.GetInt2());
		break;
	case WM_MBUTTONUP:
		OnMouseUp(EMouseButton::Middle, Message.GetInt2());
		break;
	case WM_MOUSELEAVE:
		OnMouseLeave(Message.GetInt2());
		break;
	case WM_MOUSEWHEEL:
		OnMouseWheel(Message.GetWheelDelta(), Message.GetInt2());
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(Message.GetInt2());
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
	GetWindow()->GetViewport()->Clear(float3(0.5));
	GetWindow()->GetViewport()->RenderingStart();
	Renderer->SetDrawArea(Area);
	Renderer->DrawRectangle(intrect(int2(0), Area.Size), _BackColor);
	OnRendering();
	GetWindow()->GetViewport()->RenderingEnd();
}

FUIControlContainer::FUIControlContainer()
	: FocusedContent(nullptr)
{
}

FUIControl* GetContentInRange(vector<FUIControl*> const& Contents, int2 Position)
{
	for (auto Content : Contents)
	{
		if (Content->GetArea().IsRange(Position))
		{
			return Content;
		}
	}
	return nullptr;
}

void FUIControlContainer::CallMouseDownEvent(EMouseButton Button, int2 MousePosition)
{	
	OnMouseDown(Button, MousePosition);

	auto NewFocusedContent = GetContentInRange(Contents, MousePosition);
	if (NewFocusedContent)
	{
		if (FocusedContent != NewFocusedContent)
		{
			if (FocusedContent) FocusedContent->OnLostFocus();
			NewFocusedContent->OnGetFocus();
			FocusedContent = NewFocusedContent;
		}
		FocusedContent->OnMouseDown(Button, MousePosition);
	}	
};

void FUIControlContainer::SetWindow(FUIWindow* InWindow)
{
	Window = InWindow;
	OnWindowChanged();
	for (auto Content : Contents) Content->SetWindow(Window);
}

void FUIControlContainer::SetArea(intrect InArea)
{
	Area = _Margin.CuttedOut(InArea);
	OnAreaChanged();
	for (auto Content : Contents) Content->SetArea(Area);
}

void FUIControlContainer::Initialize(FUIApplication* InApplication)
{
	Renderer = InApplication->GetRenderingManager();
	OnInitialize(InApplication);
	for (auto Content : Contents) Content->Initialize(InApplication);
}

void FUIControlContainer::ReceiveMessage(SUIWinAPIMessage const& Message)
{
	FUIControl* ContentInRange;

	switch (Message.Message)
	{
	case WM_CHAR:
		OnCharInputed(Message.GetChar());
		if (FocusedContent) FocusedContent->OnCharInputed(Message.GetChar());
		break;
	case WM_KEYDOWN:
		OnKeyDown(Message.GetChar());
		if (FocusedContent) FocusedContent->OnKeyDown(Message.GetChar());
		break;
	case WM_KEYUP:
		OnKeyUp(Message.GetChar());
		if (FocusedContent) FocusedContent->OnKeyUp(Message.GetChar());
		break;
	case WM_LBUTTONDOWN:		
		CallMouseDownEvent(EMouseButton::Left, Message.GetInt2());
		break;
	case WM_RBUTTONDOWN:
		CallMouseDownEvent(EMouseButton::Right, Message.GetInt2());
		break;
	case WM_MBUTTONDOWN:
		CallMouseDownEvent(EMouseButton::Middle, Message.GetInt2());
		break;
	case WM_LBUTTONUP:
		OnMouseUp(EMouseButton::Left, Message.GetInt2());
		if (FocusedContent) FocusedContent->OnMouseUp(EMouseButton::Left, Message.GetInt2());
		break;
	case WM_RBUTTONUP:
		OnMouseUp(EMouseButton::Right, Message.GetInt2());
		if (FocusedContent) FocusedContent->OnMouseUp(EMouseButton::Left, Message.GetInt2());
		break;
	case WM_MBUTTONUP:
		OnMouseUp(EMouseButton::Middle, Message.GetInt2());
		if (FocusedContent) FocusedContent->OnMouseUp(EMouseButton::Left, Message.GetInt2());
		break;
	case WM_MOUSELEAVE:
		OnMouseLeave(Message.GetInt2());
		if (FocusedContent) FocusedContent->OnMouseLeave(Message.GetInt2());
		break;
	case WM_MOUSEWHEEL:
		OnMouseWheel(Message.GetWheelDelta(), Message.GetInt2());
		ContentInRange = GetContentInRange(Contents, Message.GetInt2());
		ContentInRange->OnMouseWheel(Message.GetWheelDelta(), Message.GetInt2());
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(Message.GetInt2());
		if (GetIsPressed())
		{
			if (FocusedContent) FocusedContent->OnMouseMove(Message.GetInt2());
		}
		else
		{
			ContentInRange = GetContentInRange(Contents, Message.GetInt2());
			ContentInRange->OnMouseMove(Message.GetInt2());
		}
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

void FUIControlContainer::Update()
{
	GetWindow()->GetViewport()->Clear(float3(0.5));
	GetWindow()->GetViewport()->RenderingStart();
	Renderer->SetDrawArea(Area);
	Renderer->DrawRectangle(intrect(int2(0), Area.Size), _BackColor);
	OnRendering();
	for (auto Content : Contents) Content->OnRendering();
	GetWindow()->GetViewport()->RenderingEnd();
}