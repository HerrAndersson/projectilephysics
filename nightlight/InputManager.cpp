#include "InputManager.h"
#include <iostream>

InputManager::InputManager(HWND hwnd, int screenWidth, int screenHeight)
{
	this->hwnd = hwnd;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
}

InputManager::~InputManager()
{

}

void InputManager::Update()
{
	leftMouse.Update();
	esc.Update();
}

void InputManager::HandleMouse()
{
	LPPOINT point = new POINT();
	GetCursorPos(point);

	ScreenToClient(hwnd, point);

	mousePos.x = (float)point->x;
	mousePos.y = (float)point->y;

	if (mousePos.x < 0)  { mousePos.x = 0; }
	if (mousePos.y < 0)  { mousePos.y = 0; }
	if (mousePos.x  > screenWidth)  { mousePos.x = (float)screenWidth; }
	if (mousePos.y  > screenHeight) { mousePos.y = (float)screenHeight; }

	delete point;
}

XMFLOAT2 InputManager::GetMousePos()
{
	return mousePos;
}

XMFLOAT2 InputManager::GetMousePosScreenSpace()
{
	//Converting mouse position to screen space range [-1, 1]
	float x = 2 * (mousePos.x / screenWidth) - 1;
	float y = -(2 * (mousePos.y / screenHeight) - 1);
	
	return XMFLOAT2(x, y);
}

bool InputManager::KeyDown(char key)
{
	int keyToCheck = -1;

	switch (key)
	{
		case 'w':
		{
			keyToCheck = 0x57;
			break;
		}
		case 'a':
		{
			keyToCheck = 0x41;
			break;
		}
		case 's':
		{
			keyToCheck = 0x53;
			break;
		}
		case 'd':
		{
			keyToCheck = 0x44;
			break;
		}
		case 'e':
		{
			keyToCheck = 0x45;
			break;
		}
		case 'z':
		{
			keyToCheck = 0x5A;
			break;
		}
		case 'x':
		{
			keyToCheck = 0x58;
			break;
		}
		default:
			break;
	}

	if (keyToCheck != -1)
		return GetAsyncKeyState(keyToCheck) != 0;
	else
		return false;
}
bool InputManager::EscDown()
{
	return GetAsyncKeyState(VK_ESCAPE) != 0;
}
bool InputManager::EscClicked()
{
	if (esc.lastState == Key::KeyStates::DOWN && GetAsyncKeyState(esc.key) == 0)
		return true;

	return false;
}
bool InputManager::LeftMouseDown()
{
	return GetAsyncKeyState(VK_LBUTTON) != 0;
}
bool InputManager::LeftMouseClicked()
{
	if (leftMouse.lastState == Key::KeyStates::DOWN && GetAsyncKeyState(leftMouse.key) == 0)
		return true;

	return false;
}
bool InputManager::RightMouse()
{
	return GetAsyncKeyState(VK_RBUTTON) != 0;
}
bool InputManager::Space()
{
	return GetAsyncKeyState(VK_SPACE) != 0;
}