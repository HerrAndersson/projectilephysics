#pragma once
#include <DirectXMath.h>
using DirectX::XMFLOAT3;

//Movement
const float ACCELERATION				= 0.005f;
const float DECELERATION				= 0.0007f;
const float SPEED_MULTIPLIER			= 0.07f;
const float LOOK_SPEED					= 0.5f;
const float VIEW_BOUNDS				    = 75.0f;
const float CAMERA_HEIGHT_OFFSET		= 8.0f;

const float CANNON_PITCH_SPEED			= 0.4f;

//Physics
const XMFLOAT3 GRAVITY					= XMFLOAT3(0, -9.82114f, 0); //Karlskrona
//const double AIR_RESISTANCE			= 

const float IRON_DENSITY				= 7874; //kg/m3