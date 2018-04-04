#include "..\..\Public\Framework\UIApplication.h"
#include "..\..\Public\Framework\UIControl.h"
#include "..\..\Public\Framework\UIWindow.h"
#include "UIWinAPIMessage.h"
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
			WinMessage.LParameter = LParameter;
			WinMessage.WParameter = WParameter;
			Window->GetContent()->ReceiveMessage(WinMessage);
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
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
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