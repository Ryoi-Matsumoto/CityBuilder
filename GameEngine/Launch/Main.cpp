#include "UserInterface\Public\Framework\UIApplication.h"
#include "UserInterface\Public\Framework\UIWindow.h"
#include "UserInterface\Public\Control\UITextBlock.h"
#include <Windows.h>
#include "Core\Public\Script\Json.h"
#include "Core\Public\Script\Lexer.h"
#include "Core\Public\Script\Parser.h"
#include "Test\Test.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{	
	AllTest();



	return 0;
	auto Application = make_unique<FUIApplication>(hInstance, L"Test");	
	auto TextEditor = unique_ptr<FUIControl>(new FUITextEditor());
	auto Window = Application->CreateUIWindow(move(TextEditor));

	while (Application->Update());
}