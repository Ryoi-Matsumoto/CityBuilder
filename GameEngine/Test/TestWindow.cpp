#include "TestWindow.h"

LRESULT CALLBACK TestWindowProc(HWND WindowHandle, uint Message, WPARAM WParameter, LPARAM LParameter)
{
	return DefWindowProc(WindowHandle, Message, WParameter, LParameter);
}

FTestWindow::FTestWindow()
{
	HINSTANCE HInstance = nullptr;

	WNDCLASSEX WindowClass = { 0 };
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = TestWindowProc;
	WindowClass.hInstance = HInstance;
	WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	WindowClass.lpszClassName = L"Test";
	RegisterClassEx(&WindowClass);

	HWindow = CreateWindow
	(
		L"Test",
		L"Test",
		WS_POPUP,
		(1920 - Width) / 2,
		(1080 - Height) / 2,
		Width,
		Height,
		nullptr,
		nullptr,
		HInstance,
		nullptr
	);

	ShowWindow(HWindow, SW_SHOW);
}

bool FTestWindow::Update()
{
	MSG Message;
	if (GetMessage(&Message, nullptr, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return Message.message != WM_QUIT;
}
