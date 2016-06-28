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
	rightMouse.Update();
	space.Update();
	middleMouse.Update();
}

void InputManager::HandleMouse()
{
	lastMousePos = mousePos;

	POINT point;
	GetCursorPos(&point);

	RECT rect;
	GetClientRect(hwnd, &rect);
	MapWindowPoints(hwnd, GetParent(hwnd), (LPPOINT)&rect, 2);

	float sx = float(rect.left + rect.right) / 2.0f;
	float sy = float(rect.top + rect.bottom) / 2.0f;
	SetCursorPos((int)sx, (int)sy);

	float dx = point.x - sx;
	float dy = point.y - sy;

	deltaPos = XMFLOAT2(dx, dy);

	if (mousePos.x < 0)  { mousePos.x = 0; }
	if (mousePos.y < 0)  { mousePos.y = 0; }
	if (mousePos.x  > screenWidth)  { mousePos.x = (float)screenWidth; }
	if (mousePos.y  > screenHeight) { mousePos.y = (float)screenHeight; }
}

bool InputManager::MouseMoved(XMFLOAT2& difference)
{
	if (std::abs(deltaPos.x) < std::numeric_limits<float>::epsilon() && std::abs(deltaPos.y) < std::numeric_limits<float>::epsilon())
	{
		difference.x = 0;
		difference.y = 0;

		return false;
	}
	else
	{
		difference = deltaPos;
		return true;
	}
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

bool InputManager::RightMouseDown()
{
	return GetAsyncKeyState(VK_RBUTTON) != 0;
}

bool InputManager::RightMouseClicked()
{
	if (rightMouse.lastState == Key::KeyStates::DOWN && GetAsyncKeyState(rightMouse.key) == 0)
		return true;

	return false;
}

bool InputManager::MiddleMouseDown()
{
	return GetAsyncKeyState(VK_MBUTTON) != 0;
}

bool InputManager::MiddleMouseClicked()
{
	if (middleMouse.lastState == Key::KeyStates::DOWN && GetAsyncKeyState(middleMouse.key) == 0)
		return true;

	return false;
}

bool InputManager::SpaceDown()
{
	return GetAsyncKeyState(VK_SPACE) != 0;
}

bool InputManager::SpaceClicked()
{
	if (space.lastState == Key::KeyStates::DOWN && GetAsyncKeyState(space.key) == 0)
		return true;

	return false;
}

bool InputManager::UpArrowDown()
{
	return GetAsyncKeyState(VK_UP) != 0;
}

bool InputManager::DownArrowDown()
{
	return GetAsyncKeyState(VK_DOWN) != 0;
}

bool InputManager::LeftArrowDown() 
{
	return GetAsyncKeyState(VK_LEFT) != 0;
}

bool InputManager::RightArrowDown()
{
	return GetAsyncKeyState(VK_RIGHT) != 0;
}

bool InputManager::ShiftDown()
{
	return GetAsyncKeyState(VK_SHIFT) != 0;
}