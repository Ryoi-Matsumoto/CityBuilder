#include "UserInterface\Private\UIApplication.h"
#include "UserInterface\Private\UIWindow.h"
#include "UserInterface\Private\Complex\UICodeEditor.h"
#include "UserInterface\Private\Primitive\UIFormula.h"
#include "UserInterface\Private\Container\UIGrid.h"
#include <Windows.h>
#include "Core\Public\Script\Json.h"
#include "Core\Public\Script\Lexer.h"
#include "Core\Public\Script\Parser.h"
#include "Test\Test.h"
#include "Math\Public\MExpression.h"

#include "UserInterface\Private\Dock\UIUpperTab.h"


#include <codecvt> 

#define MATH_CALC

#ifdef MATH_CALC

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	auto Application = make_unique<FUIApplication>(hInstance, L"Test");
	SUIWindowStartupState State;
	State.Title = L"Window";
	State.Default();
	auto CodeEditor = new FUICodeEditor();
	CodeEditor->SetYIndex(0);
	auto Formula = new FUIFormula();
	Formula->SetYIndex(1);
	CodeEditor->GetTextBox()->OnTextChanged = [=](wstring Text) 
	{
		wstring_convert<codecvt_utf8<wchar_t>, wchar_t> Converter;
		FExpression Expression(Converter.to_bytes(Text));
		Formula->SetExpression(Expression);
		Formula->Update();
	};

	auto Grid = new FUIGrid();
	Grid->AddYDefine(SUILenght(1, true));
	Grid->AddYDefine(SUILenght(10, true));
	Grid->AddChild(CodeEditor);
	Grid->AddChild(Formula);

	auto Window = Application->CreateUIWindow(Grid, State);

	while (Application->Update());
	return 0;
}

#else

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{	
	//AllTest();
	//return 0;

	auto Application = make_unique<FUIApplication>(hInstance, L"Test");
	
	auto TabPanel = new FUIDockingPanel();
	auto Editor = new FUICodeEditor();
	auto Editor2 = new FUICodeEditor();
	
	TabPanel->AddItem(Editor, L"Code1");
	TabPanel->AddItem(Editor2, L"Code2");

	SUIWindowStartupState State;
	State.Title = L"Window";
	State.Default();
	auto Window = Application->CreateUIWindow(TabPanel, State);

	while (Application->Update());
}
#endif