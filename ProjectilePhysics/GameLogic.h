#pragma once
#include <Windows.h>
#include <iostream>
#include <math.h>
#include "InputManager.h"
#include "Camera.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "Constants.h"
#include "Terrain.h"

using namespace std;

enum ObjectTypes { STATIC, PHYSICS, CANNON };

struct Movement
{
	float forwardSpeed = 0.0f;
	float backwardSpeed = 0.0f;
	float leftSpeed = 0.0f;
	float rightSpeed = 0.0f;
};

class GameLogic
{

private:

	InputManager*  Input;

	bool quitGame = true;

	int screenWidth;
	int screenHeight;

	Movement movement;

public:

	GameLogic(InputManager* Input);
	~GameLogic();

	bool Update(double frameTime, double gameTime, vector<GameObject*> gameObjects, Camera* camera, GameObject* skySphere, Terrain* terrain, GameObject* cannon);
	bool UpdateCamera(double frameTime, Camera* camera, Terrain* terrain);
	bool UpdatePhysicsObjects(double frameTime, vector<GameObject*> gameObjects, XMFLOAT3 cannonRotation);
	bool UpdateSky(double gameTime, GameObject* skySphere);
	bool UpdateCannon(GameObject* cannon);
};

