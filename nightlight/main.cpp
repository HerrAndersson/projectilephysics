#include <windows.h>
#include "System.h"
#include <crtdbg.h>
#include <iostream>
#include <ctime>
//#include "vld.h"

//#if DEBUG
//#include "Console.h"
//#include <fstream>
//#include <conio.h>
//#include <stdio.h>
//#include <cstdlib>
//#endif

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
//#if DEBUG
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//RedirectIOToConsole();
//#endif

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