#pragma once
#include <DirectXMath.h>
using DirectX::XMFLOAT3;

//Movement
const float ACCELERATION				= 0.001f;
const float DECELERATION				= 0.0007f;
const float SPEED_MULTIPLIER			= 0.03f;
const float LOOK_SPEED					= 0.5f;
const float VIEW_BOUNDS_X				= 75.0f;
const float CAMERA_HEIGHT_OFFSET		= 8.0f;

//Physics
const XMFLOAT3 GRAVITY					= XMFLOAT3(0, -9.82114f, 0); //Karlskrona
//const double AIR_RESISTANCE			= 