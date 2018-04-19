#include "UserInterface\Private\UIApplication.h"
#include "UserInterface\Private\UIWindow.h"
#include "UserInterface\Private\Complex\UICodeEditor.h"
#include <Windows.h>
#include "Core\Public\Script\Json.h"
#include "Core\Public\Script\Lexer.h"
#include "Core\Public\Script\Parser.h"
#include "Test\Test.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{	
	//AllTest();
	//return 0;

	auto Application = make_unique<FUIApplication>(hInstance, L"Test");	
	auto TextEditor = unique_ptr<FUIControl>(new FUICodeEditor());
	auto Window = Application->CreateUIWindow(move(TextEditor));

	while (Application->Update());
}