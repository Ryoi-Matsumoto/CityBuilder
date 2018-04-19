#include "UIContainer.h"
#include <Windows.h>

FUIContainer::FUIContainer()
	: FocusedChild(nullptr)
{
}

FUIControl* GetChildInRange(vector<unique_ptr<FUIControl>> const& Childs, int2 Position)
{
	for (auto& Child : Childs)
	{
		if (Child->GetArea().IsInRange(Position))
			return Child.get();
	}

	return nullptr;
}

void FUIContainer::SetWindow(FUIWindow* InWindow)
{
	Window = InWindow;
	OnWindowChanged();

	for (auto& Child : Children)
		Child->SetWindow(Window);
}

void FUIContainer::Initialize(FUIApplication* InApplication)
{
	Renderer = InApplication->GetRenderingManager();
	OnInitialize(InApplication);

	for (auto& Child : Children)
		Child->Initialize(InApplication);
}

void FUIContainer::ReceiveMessage(SUIWinAPIMessage const& Message)
{
	FUIControl::ReceiveMessage(Message);

	FUIControl* NewTargetChild;
	switch (Message.Message)
	{
	case WM_CHAR:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_MOUSELEAVE:
	case WM_LBUTTONDBLCLK:
		if (FocusedChild)
			FocusedChild->ReceiveMessage(Message);
		break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		NewTargetChild = GetChildInRange(Children, Message.GetInt2());
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
		InRangeChild = GetChildInRange(Children, Message.GetInt2());
		if (InRangeChild)
			InRangeChild->ReceiveMessage(Message);
		break;
	case WM_MOUSEMOVE:
		OnMouseMove(Message.GetInt2());
		if (GetMouseButtonDown(EMouseButton::Left) && FocusedChild)
		{
			FocusedChild->ReceiveMessage(Message);
		}
		else
		{
			NewTargetChild = GetChildInRange(Children, Message.GetInt2());
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
}

void FUIContainer::Update()
{
	FUIControl::Update();
	for (auto& Child : Children)
		Child->Update();
}

void FUIContainer::UpdateArea()
{
	OnAreaChanged();
	Update();
}

void FUIContainer::AddChild(FUIControl* Child)
{ 
	Child->Parent = this;
	Children.emplace_back(Child); 
}