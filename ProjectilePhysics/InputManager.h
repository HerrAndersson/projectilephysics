#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <string>
using namespace DirectX;

struct Key
{
	enum KeyStates { DOWN, UP };
	char key;
	int lastState = UP;

	Key(char key)
	{ 
		this->key = key; 
	}

	void Update()
	{
		if (GetAsyncKeyState(key) != 0)
			lastState = DOWN;
		else
			lastState = UP;
	}
};

class InputManager
{
private:

	HWND hwnd;
	int screenWidth, screenHeight;

	Key leftMouse = Key(VK_LBUTTON);
	Key rightMouse = Key(VK_RBUTTON);
	Key middleMouse = Key(VK_MBUTTON);
	Key esc = Key(VK_ESCAPE);
	Key space = Key(VK_SPACE);

	XMFLOAT2 mousePos;
	XMFLOAT2 lastMousePos;
	XMFLOAT2 deltaPos;

public:

	InputManager(HWND hwnd, int screenWidth, int screenHeight);
	~InputManager();

	void Update();

	void HandleMouse();
	XMFLOAT2 GetMousePos();
	XMFLOAT2 GetMousePosScreenSpace();

	bool MouseMoved(XMFLOAT2& difference);

	bool KeyDown(char key);

	bool EscDown();
	bool EscClicked();

	bool LeftMouseDown();
	bool LeftMouseClicked();

	bool RightMouseDown();
	bool RightMouseClicked();

	bool MiddleMouseDown();
	bool MiddleMouseClicked();

	bool SpaceDown();
	bool SpaceClicked();

	bool UpArrowDown();
	bool DownArrowDown();
	bool LeftArrowDown();
	bool RightArrowDown();

	bool ShiftDown();
};

