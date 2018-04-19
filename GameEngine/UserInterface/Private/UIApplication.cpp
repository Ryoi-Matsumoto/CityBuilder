#include "UIApplication.h"
#include "UIControl.h"
#include "UIWindow.h"
#include <Windows.h>

vector<FUIWindow*> Windows;

LRESULT CALLBACK WindowProc(HWND WindowHandle, uint Message, WPARAM WParameter, LPARAM LParameter)
{
	auto WindowIndex = GetWindowLongPtr(WindowHandle, GWLP_USERDATA);
	if (WindowIndex < Windows.size())
	{
		auto Window = Windows[WindowIndex];

		if (Message == WM_DESTROY)
		{
			PostQuitMessage(0);
		}
		else
		{
			SUIWinAPIMessage WinMessage;
			WinMessage.Message = Message;
			WinMessage.LParameter = max(LParameter, 0);
			WinMessage.WParameter = WParameter;

			Window->ReceiveMessage(WinMessage);
		}
	}
	return DefWindowProc(WindowHandle, Message, WParameter, LParameter);
}

FUIApplication::FUIApplication(HINSTANCE InstanceHandle, wstring const& ApplicationName)
	: InstanceHandle(InstanceHandle)
	, ApplicationName(ApplicationName)
{
	WNDCLASSEX WindowClass = { 0 };
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS/*ダブルクリック*/;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = InstanceHandle;
	WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	WindowClass.lpszClassName = &ApplicationName[0];
	RegisterClassEx(&WindowClass);

	RenderingManager = make_unique<FRManager>();
	
	SRFontType FontType;
	DefaultFontSet = RenderingManager->GetFontSet(FontType);
}

bool FUIApplication::Update()
{
	MSG Message;
	if (GetMessage(&Message, nullptr, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return Message.message != WM_QUIT;
}

unique_ptr<FUIWindow> FUIApplication::CreateUIWindow(unique_ptr<FUIControl> Content)
{
	auto Window = make_unique<FUIWindow>(this, move(Content));
	SetWindowLongPtr(Window->GetWindowHandle(), GWLP_USERDATA, Windows.size());
	Windows.push_back(Window.get());
	return Window;
}

float4 FUIApplication::GetColor(EUIColorType Type) const
{
	switch (Type)
	{
	case EUIColorType::Base:
		return float4(0.15f, 1);
	case EUIColorType::Selection:
		return float4::FromRGBChar(0, 122, 204);
	case EUIColorType::Font:
		return float4(1);
	case EUIColorType::Button:
		return float4(0, 0, 0, 1);
	case EUIColorType::ButtonOnCursor:
		return float4(0, 0, 0, 1);
	case EUIColorType::ButtonOnClick:
		return  float4::FromRGBChar(158, 158, 158);
	case EUIColorType::ScrollBack:
		return float4(0.2f, 1);
	case EUIColorType::Scroll:
		return float4(0.4f, 1);
	case EUIColorType::ScrollOnMouse:
		return float4(0.6f, 1);
	case EUIColorType::ScrollOnClick:
		return float4(0.8f, 1);
	case EUIColorType::LineNumber:
		return float4::FromRGBChar(170, 102, 205);
	default:
		assert(false);
		return float4(0, 0, 1, 1);
	}
}
