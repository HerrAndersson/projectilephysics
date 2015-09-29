#pragma once
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdexcept>
#include "Game.h"
#include "Timer.h"
#include "Cpu.h"

class System
{

private:

	const bool debugShowFps = true;
	//const double MS_PER_FRAME = (1000 / 62.0f); //62FPS
	const double MS_PER_FRAME = (1000 / 60.0f);//60FPS
	//const double MS_PER_FRAME =  (1000 / 120.0f); //120FPS
	//const double MS_PER_FRAME =  (1000 / 144.0f); //144FPS

	struct FPS
	{
		DWORD frames;
		DWORD currentTime;
		DWORD lastUpdateTime;
		DWORD elapsedTime;
		DWORD fps;

		FPS()
		{
			frames = 0;
			currentTime = 0;
			lastUpdateTime = 0;
			elapsedTime = 0;
			fps = 0;
		}

		void Update()
		{
			frames++;
			currentTime = GetTickCount(); // Even better to use timeGetTime()
			elapsedTime = currentTime - lastUpdateTime;
			if (elapsedTime >= 1000)
			{
				fps = (UINT)(frames * 1000.0 / elapsedTime);
				frames = 0;
				lastUpdateTime = currentTime;
			}
		}
	};

	LPCSTR		applicationName;
	HINSTANCE	hinstance;
	HWND		hwnd;

	Game*		game;
	Timer*		timer;
	FPS			fps;
	Cpu*        cpuUsage;

	int windowWidth, windowHeight;
	int screenWidth, screenHeight;

	bool fullscreen;
	bool showCursor;

	bool Update();
	void InitializeWindows();
	void ShutdownWindows();

public:

	System(bool fullscreen = false, bool showCursor = true, int windowWidth = 1000, int windowHeight = 800);
	~System();

	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static System* gameHandle = nullptr;