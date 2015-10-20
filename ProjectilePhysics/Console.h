#ifndef __GUICON_H__
#define __GUICON_H__

#include <windows.h>
#include <iostream>

//maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

void RedirectIOToConsole()
{
	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	//Allocate a console for this app
	AllocConsole();
	SetConsoleTitle("Debug console");

	//Set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = MAX_CONSOLE_LINES;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);

	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();
}

#endif
