#pragma once
#include <Windows.h>
#include <iostream>
#include <math.h>
#include "InputManager.h"
#include "Camera.h"


class GameLogic
{

private:

	InputManager*  Input;

	bool quitGame = true;

	int screenWidth;
	int screenHeight;

	bool leftMouseLastState = false;

public:

	GameLogic(InputManager* Input);
	~GameLogic();

	//Should receive GameTime
	bool Update();
};

