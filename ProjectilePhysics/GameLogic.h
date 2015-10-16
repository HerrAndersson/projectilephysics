#pragma once
#include <Windows.h>
#include <iostream>
#include <math.h>
#include "InputManager.h"
#include "Camera.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "GameUtil.h"
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
	Movement movement;

	int screenWidth;
	int screenHeight;

	double startTime;

	float cannonLaunchSpeed;
	bool airResistanceOn;
	bool responseForceOn;

	XMFLOAT3 CalculateVelocity(float launchSpeed, XMFLOAT3 cannonRotation);

public:

	GameLogic(InputManager* Input);
	~GameLogic();

	bool Update(double frameTime, double gameTime, vector<PhysicsObject*>& projectiles, Camera* camera, GameObject* skySphere, Terrain* terrain, GameObject* cannon);
	bool UpdateCamera(double frameTime, Camera* camera, Terrain* terrain);
	bool UpdatePhysicsObjects(double frameTime, double gameTime, vector<PhysicsObject*>& projectiles, XMFLOAT3 cannonRotation, Terrain* terrain);
	bool UpdateSky(double gameTime, GameObject* skySphere);
	bool UpdateCannon(GameObject* cannon);

	float GetLaunchSpeed();
	bool AirResistanceOn();
	bool ResponseForceOn();
};

