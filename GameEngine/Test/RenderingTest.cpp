#include "Test.h"
#include "TestWindow.h"
#include "Rendering\Public\RManager.h"
#include <Windows.h>

void Rendering2DTest()
{
	FTestWindow Window;	
	FRManager Manager;
	auto Viewport = Manager.CreateViewport(Window.GetHWindow(), Window.GetSize());
	
	Viewport->RenderingStart();
	Manager.SetDrawArea(intrect(0, Window.GetSize()));
	Manager.DrawRectangle(intrect(int2(200, 100), int2(200, 100)), float4(1, 0, 0, 1));
	Viewport->RenderingEnd();	

	while (Window.Update());
}