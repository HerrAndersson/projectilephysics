#include "System.h"

System::System(bool fullscreen, bool showCursor, int windowWidth, int windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	this->fullscreen = fullscreen;
	this->showCursor = showCursor;

	InitializeWindows();

	//Create and initialize the application

	timer = new Timer();
	cpuUsage = new Cpu();

	game = new Game(hinstance, hwnd, this->windowWidth, this->windowHeight, fullscreen);
}


System::~System()
{
	delete game;
	delete timer;
	delete cpuUsage;
	ShutdownWindows();
}

void System::Run()
{
	MSG msg;
	bool result;
	bool done = false;

	ZeroMemory(&msg, sizeof(MSG));

	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Update();
			if (!result)
			{
				done = true;
			}
		}
	}
}


bool System::Update()
{
	bool result = true;

	timer->Update();
	cpuUsage->Update();

	if (timer->GetGameTime() > MS_PER_FRAME)
	{
		if (GetFocus() == hwnd)
		{
			result = game->Update();
			if (!result) { return false; }
		}

		result = game->Render();
		if (!result) { return false; }

		std::string s = "nightlight";
		if (game->GetDebugShowFps()){
			s += " - msPerSystemUpdate: " + std::to_string(timer->GetFrameTime())
				+ " msPerFrame: " + std::to_string(timer->GetGameTime())
				+ " SystemUpdatePerSecond : " + std::to_string(timer->GetFPS())
				+ " CPU% : " + std::to_string(cpuUsage->GetCpuPercentage());
		}

		SetWindowText(hwnd, s.c_str());

		timer->Reset();
	}

	return result;
}


LRESULT CALLBACK System::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


void System::InitializeWindows()
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	gameHandle = this;

	hinstance = GetModuleHandle(NULL);
	applicationName = "nightlight";

	//Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	//Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (fullscreen)
	{
		//Determine the resolution of the screen.
		screenWidth = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);
		windowHeight = screenHeight;
		windowWidth = screenWidth;

		//If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = 0;
		posY = 0;
	}
	else //If windowed
	{
		//Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;

		screenHeight = GetSystemMetrics(SM_CYSCREEN);
		screenWidth = GetSystemMetrics(SM_CXSCREEN);
	}

	RECT rc = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
		WS_OVERLAPPEDWINDOW | CW_USEDEFAULT | CW_USEDEFAULT,
		posX, posY, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hinstance, NULL);

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	ShowCursor(showCursor);
}


void System::ShutdownWindows()
{
	ShowCursor(true);

	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	SetCursorPos(screenWidth / 2, screenHeight / 2);

	DestroyWindow(hwnd);
	hwnd = NULL;

	UnregisterClass(applicationName, hinstance);
	hinstance = NULL;

	gameHandle = NULL;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			return gameHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}