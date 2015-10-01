#pragma once
#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;

struct Key
{
	enum KeyStates { DOWN, UP };
	char key;
	int lastState = UP;

	Key(char key){ this->key = key; }
	void Update(){
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
	Key esc = Key(VK_ESCAPE);

	XMFLOAT2 mousePos;

public:

	InputManager(HWND hwnd, int screenWidth, int screenHeight);
	~InputManager();

	void Update();

	void HandleMouse();
	XMFLOAT2 GetMousePos();
	XMFLOAT2 GetMousePosScreenSpace();

	bool KeyDown(char key);
	bool EscDown();
	bool EscClicked();
	bool LeftMouseDown();
	bool LeftMouseClicked();
	bool RightMouse();
	bool Space();
};

