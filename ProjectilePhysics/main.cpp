#define _CRTDBG_MAP_ALLOC
#include <windows.h>
#include "System.h"
#include <iostream>
#include <ctime>
//#include "vld.h"
#include "Console.h"

/*

Add Quad-tree culling: http://www.rastertek.com/tertut05.html
Add small body of water
Moving : http://stackoverflow.com/questions/25605659/avoid-ground-collision-with-bullet

*/

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	RedirectIOToConsole();

	srand((unsigned)time(NULL));

	System* system;

	try
	{
		system = new System(false, true, 1280, 720);
		system->Run();
		delete system;
	}
	catch (exception& e)
	{
		MessageBoxA(NULL, e.what(), "Error", MB_ICONERROR | MB_OK);
	}

	return 0;
}