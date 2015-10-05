#pragma once
#include <DirectXMath.h>
using DirectX::XMFLOAT3;

namespace MovementConstants
{
	const float ACCELERATION = 0.005f;
	const float DECELERATION = 0.0007f;
	const float SPEED_MULTIPLIER = 0.07f;
	const float SHIFT_SPEED_MULTIPLIER = 0.35f;
	const float LOOK_SPEED = 0.5f;
	const float VIEW_BOUNDS = 75.0f;
	const float CAMERA_HEIGHT_OFFSET = 8.0f;
}

namespace PhysicsConstants
{
	const XMFLOAT3 GRAVITY = XMFLOAT3(0, -9.82114f, 0); //Karlskrona
	const float IRON_DENSITY = 7874; //kg/m3
	//const double AIR_RESISTANCE			= 
}

namespace GameConstants
{
	const int MAX_LAUNCH_SPEED = 80;
	const int WORLD_SIZE = 1024;
	const int NUMBER_OF_CANNONBALLS = 20;
	const XMFLOAT3 CANNONBALL_START_POS = XMFLOAT3(480, 20, 140);
	const float CANNON_PITCH_SPEED = 0.4f;
}