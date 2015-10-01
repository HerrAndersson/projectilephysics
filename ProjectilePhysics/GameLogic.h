#pragma once
#include <Windows.h>
#include <iostream>
#include <math.h>
#include "InputManager.h"
#include "Camera.h"
#include "GameObject.h"
#include "PhysicsObject.h"

using namespace std;

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

	bool Update(double gameTime, vector<GameObject*> gameObjects, Camera* camera);
};

