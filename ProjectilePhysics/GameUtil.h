#pragma once
#include <DirectXMath.h>
using DirectX::XMFLOAT3;


const int UNITS_PER_METER = 10;

static float UnitsToMeter(float units)
{
	return units / UNITS_PER_METER;
}

static float MeterToUnits(float meter)
{
	return meter * UNITS_PER_METER;
}

static void TruncateOne(float x, int& x1, int& x2)
{
	x1 = (int)x;
	float z = x - x1;
	z = z * 10;
	x2 = (int)z;
}

namespace MovementConstants
{
	const float ACCELERATION = 0.005f;
	const float DECELERATION = 0.007f;
	const float SPEED_MULTIPLIER = 0.07f;
	const float SHIFT_SPEED_MULTIPLIER = 0.35f;
	const float LOOK_SPEED = 0.5f;
	const float VIEW_BOUNDS = 75.0f;
	const float CAMERA_HEIGHT_OFFSET = MeterToUnits(1.8f);
}

namespace PhysicsConstants
{
	const XMFLOAT3 GRAVITY = XMFLOAT3(0, -9.82114f, 0); //Karlskrona

	const float DRAG_COEFF_SPHERE = 0.47f;
	const float DRAG_COEFF_CUBE = 1.05f;

	const float IRON_DENSITY = 7870; //Kg/m^3
	const float AIR_DENSITY = 1.204f; // Kg/m^3 in 20 celsius
}

namespace GameConstants
{
	const int WORLD_SIZE = 1024;

	const float MAX_LAUNCH_SPEED = 250.0f;
	const float MIN_LAUNCH_SPEED = 10.1f;
	const float INITIAL_LAUNCH_SPEED = 20;
	const int NUMBER_OF_CANNONBALLS = 20;
	const XMFLOAT3 CANNONBALL_START_POS = XMFLOAT3(480, 15, 135);
	const float CANNON_PITCH_SPEED = 0.1f;
}