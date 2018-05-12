#include "UIControl.h"
#include <Windows.h>

FUIControl::FUIControl()
	: XIndex(0)
	, YIndex(0)
	, Margin(0)
	, BackColor(EUIColorType::Base)
	, IsInitialized(false)
	, IsAreaSetted(false)
	, IsVisible(true)
	, Parent(nullptr)
	, BindControl(nullptr)
	, FocusedChild(nullptr)
	, InRangeChild(nullptr)
	, Window(nullptr)
{
	for (uint i = 0; i < 3; i++)
		MouseButtonStates[i] = false;
}

FUIControl* FUIControl::GetChildInRange(int2 Position)
{
	for (auto& Child : Children)
	{
		if (Child->Area.IsInRange(Position))
			return Child.get();
	}

	return nullptr;
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
			if (!TimerControl)
				return;
			auto Window = TimerControl->GetWindow();
			if (Window)
			{
				Window->GetViewport()->RenderingStart();
				TimerControl->OnTimer();
				Window->GetViewport()->RenderingEnd();
			}
		}
	};

	auto Window = GetWindow();
	if (Window)
	{
		TimerControl = this;
		::SetTimer(Window->GetWindowHandle(), 0, Millisecond, Proc::TimerProcedure);
	}
}

void FUIControl::SetArea(intrect InArea)
{
	IsAreaSetted = true;
	Area = InArea;
	OnAreaChanged();
}

void FUIControl::Initialize(FUIApplication* InApplication)
{
	if (IsInitialized)
		return;

	IsInitialized = true;
	Application = InApplication;
	OnInitialize();

	for (auto& Child : Children)
		Child->Initialize(Application);
}

LRESULT FUIControl::ReceiveMessage(SUIWinAPIMessage const& Message)
{
	int2 MousePosition = Message.GetInt2() - Area.Position + int2(Margin.Left, Margin.Top);
	
	switch (Message.Message)
	{
	case WM_NCHITTEST:
		if (!GetIsClientArea())
			return HTCAPTION;
		break;
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

	if (Children.size() == 0)
		return 0;

	FUIControl* NewTargetChild;
	switch (Message.Message)
	{
	case WM_NCHITTEST:
		// CAUTION !
		// when WM_NCHITTEST, the mouse coordinates are not window coordinates but desktop coordinates
		NewTargetChild = GetChildInRange(Message.GetInt2() - GetWindow()->GetWindowArea().Position);
		if (NewTargetChild)
			return NewTargetChild->ReceiveMessage(Message);
		break;
	case WM_CHAR:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_LBUTTONDBLCLK:
		if (FocusedChild)
			FocusedChild->ReceiveMessage(Message);
		break;
	case WM_MOUSELEAVE:
		if (InRangeChild)
		{
			auto LeaveMessage = Message;
			LeaveMessage.Message = WM_MOUSELEAVE;
			InRangeChild->ReceiveMessage(LeaveMessage);
			InRangeChild = nullptr;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		NewTargetChild = GetChildInRange(Message.GetInt2());
		if (NewTargetChild)
		{
			if (FocusedChild != NewTargetChild)
			{
				if (FocusedChild)
					FocusedChild->OnLostFocus();

				NewTargetChild->OnGetFocus();
				FocusedChild = NewTargetChild;
			}
			FocusedChild->ReceiveMessage(Message);
		}
		break;
	case WM_MOUSEWHEEL:
		InRangeChild = GetChildInRange(Message.GetInt2() - GetWindow()->GetWindowArea().Position);
		if (InRangeChild)
			InRangeChild->ReceiveMessage(Message);
		break;
	case WM_MOUSEMOVE:
		if (GetMouseButtonDown(EMouseButton::Left) && FocusedChild)
		{
			FocusedChild->ReceiveMessage(Message);
		}
		else
		{
			NewTargetChild = GetChildInRange(Message.GetInt2());
			if (NewTargetChild)
			{
				if (InRangeChild != NewTargetChild)
				{
					if (InRangeChild)
					{
						auto LeaveMessage = Message;
						LeaveMessage.Message = WM_MOUSELEAVE;
						InRangeChild->ReceiveMessage(LeaveMessage);
					}
					InRangeChild = NewTargetChild;
				}
				InRangeChild->ReceiveMessage(Message);
			}
		}
		break;
	}

	return 0;
}

void FUIControl::Update()
{
	if (!IsVisible)
		return;

	GetRenderer()->SetDrawArea(Area);
	if (BackColor != EUIColorType::None)
		GetRenderer()->DrawRectangle(intrect(int2(0), Area.Size), Application->GetColor(BackColor));
	GetRenderer()->SetDrawArea(GetRenderingArea());

	OnRendering();
	for (auto& Child : Children)
		Child->Update();
}

void FUIControl::UpdateArea()
{
	OnAreaChanged();
	Update();
}

uint FUIControl::GetActualWidth() const
{
	if (Children.size() == 0)
		return 100;

	uint MaxWidth = 0;
	for (auto& Child : Children)
		MaxWidth = FMath::Max(MaxWidth, Child->GetWidth());
	return MaxWidth;
}

uint FUIControl::GetActualHeight() const
{
	if (Children.size() == 0)
		return 100;

	uint MaxHeight = 0;
	for (auto& Child : Children)
		MaxHeight = FMath::Max(MaxHeight, Child->GetHeight());
	return MaxHeight;
}

int2 FUIControl::GetRenderingSize() const
{
	return int2(Area.Size.X - Margin.Left - Margin.Right, Area.Size.Y - Margin.Top - Margin.Bottom);
}

int2 FUIControl::GetRenderingPosition() const
{
	return Area.Position + int2(Margin.Left, Margin.Top);
}

void FUIControl::AddChild(FUIControl* Child)
{
	Child->Parent = this;
	Children.emplace_back(Child);
	if (IsAreaSetted)
		OnAreaChanged();
}

void FUIControl::RemoveChild(FUIControl* Child)
{
	for (auto Itr = Children.begin(), End = Children.end(); Itr != End; Itr++)
	{
		if (Itr->get() == Child)
		{
			Child = nullptr;
			Itr->release();
			Children.erase(Itr);
			return;
		}
	}
	assert(!"not found the child");
}

void FUIControl::SetOnlyChild(FUIControl* Child)
{
	for (auto& Child : Children)
	{
		Child->Parent = nullptr;
		Child.release();
	}
	Children.clear();
	if(Child)
		AddChild(Child);
}