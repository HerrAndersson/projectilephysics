#include "GameLogic.h"

GameLogic::GameLogic(InputManager* Input)
{
	this->Input = Input;
}

GameLogic::~GameLogic()
{

}

bool GameLogic::Update(double frameTime, double gameTime, vector<GameObject*> gameObjects, Camera* camera, GameObject* skySphere, Terrain* terrain)
{
	Input->HandleMouse();
	Input->Update();

	if (!UpdateCamera(frameTime, camera, terrain))
		return false;

	if (!UpdatePhysicsObjects(frameTime, gameObjects))
		return false;

	if (!UpdateSky(gameTime, skySphere))
		return false;




	if (Input->EscDown())
		return false;

	return true;
}

bool GameLogic::UpdateCamera(double frameTime, Camera* camera, Terrain* terrain)
{

	XMFLOAT2 difference;
	XMFLOAT3 rotation = camera->GetRotation();
	XMFLOAT3 position = camera->GetPosition();
	float radians;

	///////////////////////////////////////////////  Look around  ///////////////////////////////////////////////
	if (Input->MouseMoved(difference))
	{
		rotation.y += difference.x / 2;
		rotation.x += difference.y / 2;

		////Keep y-rotation within 360-bounds
		if (rotation.y < 0.0f)
		{
			rotation.y += 360.0f;
		}
		else if (rotation.y > 360.0f)
		{
			rotation.y = 0;
		}
		//Lock x-rotation to given bounds otherwise the camera can be turned upside-down
		if (rotation.x < -75.0f)
		{
			rotation.x = -75.0f;
		}
		else if (rotation.x > 75.0f)
		{
			rotation.x = 75.0f;
		}
	}


	///////////////////////////////////////////////  Move forward  ///////////////////////////////////////////////
	if (Input->KeyDown('w'))
	{
		movement.forwardSpeed += float(frameTime * ACCELERATION);

		if (movement.forwardSpeed > (float(frameTime * SPEED_MULTIPLIER)))
			movement.forwardSpeed = float(frameTime * SPEED_MULTIPLIER);
	}
	else //If the key is not down, decelerate.
	{
		movement.forwardSpeed -= float(frameTime * DECELERATION);

		if (movement.forwardSpeed < 0.0f)
			movement.forwardSpeed = 0.0f;
	}

	radians = XMConvertToRadians(rotation.y);
	position.x += sinf(radians) * movement.forwardSpeed;
	position.z += cosf(radians) * movement.forwardSpeed;


	///////////////////////////////////////////////  Move backward  ///////////////////////////////////////////////
	if (Input->KeyDown('s'))
	{
		movement.backwardSpeed += float(frameTime * ACCELERATION);

		if (movement.backwardSpeed > (float(frameTime * SPEED_MULTIPLIER)))
			movement.backwardSpeed = float(frameTime * SPEED_MULTIPLIER);
	}
	else //If the key is not down, decelerate.
	{
		movement.backwardSpeed -= float(frameTime * DECELERATION);

		if (movement.backwardSpeed < 0.0f)
			movement.backwardSpeed = 0.0f;
	}

	radians = XMConvertToRadians(rotation.y);
	position.x -= sinf(radians) * movement.backwardSpeed;
	position.z -= cosf(radians) * movement.backwardSpeed;


	///////////////////////////////////////////////  Move left  ///////////////////////////////////////////////
	if (Input->KeyDown('a'))
	{
		movement.leftSpeed += float(frameTime * ACCELERATION);

		if (movement.leftSpeed > (float(frameTime * SPEED_MULTIPLIER)))
			movement.leftSpeed = float(frameTime * SPEED_MULTIPLIER);
	}
	else //If the key is not down, decelerate.
	{
		movement.leftSpeed -= float(frameTime * DECELERATION);

		if (movement.leftSpeed < 0.0f)
			movement.leftSpeed = 0.0f;
	}

	//rotation.y gives forward-direction. -90 gives left
	radians = XMConvertToRadians(rotation.y - 90.0f);
	position.x += sinf(radians) * movement.leftSpeed;
	position.z += cosf(radians) * movement.leftSpeed;


	///////////////////////////////////////////////  Move right  ///////////////////////////////////////////////
	if (Input->KeyDown('d'))
	{
		movement.rightSpeed += float(frameTime * ACCELERATION);

		if (movement.rightSpeed > (float(frameTime * SPEED_MULTIPLIER)))
			movement.rightSpeed = float(frameTime * SPEED_MULTIPLIER);
	}
	else //If the key is not down, decelerate.
	{
		movement.rightSpeed -= float(frameTime * DECELERATION);

		if (movement.rightSpeed < 0.0f)
			movement.rightSpeed = 0.0f;
	}

	//rotation.y gives forward-direction. +90 gives right
	radians = XMConvertToRadians(rotation.y + 90.0f);
	position.x += sinf(radians) * movement.rightSpeed;
	position.z += cosf(radians) * movement.rightSpeed;


	///////////////////////////////////////////////////  Done  ///////////////////////////////////////////////////

	position.y = terrain->GetY(position.x, position.z) + CAMERA_HEIGHT_OFFSET;
	camera->SetRotation(rotation);
	camera->SetPosition(position);

	return true;
}

bool GameLogic::UpdatePhysicsObjects(double frameTime, vector<GameObject*> gameObjects)
{
	((PhysicsObject*)(gameObjects.at(0)))->Update(frameTime);

	return true;
}

bool GameLogic::UpdateSky(double gameTime, GameObject* skySphere)
{
	XMFLOAT3 r = skySphere->GetRotation();
	r.y += 0.005f;
	skySphere->SetRotation(r);

	XMFLOAT3 colorModifier = XMFLOAT3((float)sin(gameTime / 100000), (float)sin(gameTime / 100000), (float)sin(gameTime / 100000));
	skySphere->SetColorModifier(colorModifier);

	return true;
}
