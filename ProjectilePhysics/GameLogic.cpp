#include "GameLogic.h"

GameLogic::GameLogic(InputManager* Input)
{
	this->Input = Input;
}

GameLogic::~GameLogic()
{

}

bool GameLogic::Update(double frameTime, double gameTime, vector<GameObject*>& gameObjects, Camera* camera, GameObject* skySphere, Terrain* terrain, GameObject* cannon)
{
	Input->HandleMouse();

	if (!UpdateCamera(frameTime, camera, terrain))
		return false;

	if (!UpdateCannon(cannon))
		return false;

	if (!UpdatePhysicsObjects(frameTime, gameObjects, cannon->GetRotation()))
		return false;

	if (!UpdateSky(gameTime, skySphere))
		return false;

	if (Input->EscDown())
		return false;

	Input->Update();

	return true;
}

bool GameLogic::UpdateCamera(double frameTime, Camera* camera, Terrain* terrain)
{

	XMFLOAT2 difference;
	XMFLOAT3 rotation = camera->GetRotation();
	XMFLOAT3 position = camera->GetPosition();
	float radians;

	float speedMultiplier;

	if (Input->ShiftDown())
		speedMultiplier = MovementConstants::SHIFT_SPEED_MULTIPLIER;
	else
		speedMultiplier = MovementConstants::SPEED_MULTIPLIER;

	///////////////////////////////////////////////  Look around  ///////////////////////////////////////////////
	if (Input->MouseMoved(difference))
	{
		rotation.y += difference.x / 2;
		rotation.x += difference.y / 2;

		////Keep y-rotation within 360-bounds
		if (rotation.y < 0.0f)
			rotation.y += 360.0f;
		else if (rotation.y > 360.0f)
			rotation.y = 0;

		//Lock x-rotation to given bounds otherwise the camera can be turned upside-down
		if (rotation.x < -MovementConstants::VIEW_BOUNDS)
			rotation.x = -MovementConstants::VIEW_BOUNDS;
		else if (rotation.x > MovementConstants::VIEW_BOUNDS)
			rotation.x = MovementConstants::VIEW_BOUNDS;

	}

	///////////////////////////////////////////////  Move forward  ///////////////////////////////////////////////
	if (Input->KeyDown('w'))
	{
		movement.forwardSpeed += float(frameTime * MovementConstants::ACCELERATION);

		if (movement.forwardSpeed > (float(frameTime * speedMultiplier)))
			movement.forwardSpeed = float(frameTime * speedMultiplier);
	}
	else //If the key is not down, decelerate.
	{
		movement.forwardSpeed -= float(frameTime * MovementConstants::DECELERATION);

		if (movement.forwardSpeed < 0.0f)
			movement.forwardSpeed = 0.0f;
	}

	radians = XMConvertToRadians(rotation.y);
	position.x += sinf(radians) * movement.forwardSpeed;
	position.z += cosf(radians) * movement.forwardSpeed;

	///////////////////////////////////////////////  Move backward  ///////////////////////////////////////////////
	if (Input->KeyDown('s'))
	{
		movement.backwardSpeed += float(frameTime * MovementConstants::ACCELERATION);

		if (movement.backwardSpeed > (float(frameTime * speedMultiplier)))
			movement.backwardSpeed = float(frameTime * speedMultiplier);
	}
	else //If the key is not down, decelerate.
	{
		movement.backwardSpeed -= float(frameTime * MovementConstants::DECELERATION);

		if (movement.backwardSpeed < 0.0f)
			movement.backwardSpeed = 0.0f;
	}

	radians = XMConvertToRadians(rotation.y);
	position.x -= sinf(radians) * movement.backwardSpeed;
	position.z -= cosf(radians) * movement.backwardSpeed;

	///////////////////////////////////////////////  Move left  ///////////////////////////////////////////////
	if (Input->KeyDown('a'))
	{
		movement.leftSpeed += float(frameTime * MovementConstants::ACCELERATION);

		if (movement.leftSpeed > (float(frameTime * speedMultiplier)))
			movement.leftSpeed = float(frameTime * speedMultiplier);
	}
	else //If the key is not down, decelerate.
	{
		movement.leftSpeed -= float(frameTime * MovementConstants::DECELERATION);

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
		movement.rightSpeed += float(frameTime * MovementConstants::ACCELERATION);

		if (movement.rightSpeed > (float(frameTime * speedMultiplier)))
			movement.rightSpeed = float(frameTime * speedMultiplier);
	}
	else //If the key is not down, decelerate.
	{
		movement.rightSpeed -= float(frameTime * MovementConstants::DECELERATION);

		if (movement.rightSpeed < 0.0f)
			movement.rightSpeed = 0.0f;
	}

	//rotation.y gives forward-direction. +90 gives right
	radians = XMConvertToRadians(rotation.y + 90.0f);
	position.x += sinf(radians) * movement.rightSpeed;
	position.z += cosf(radians) * movement.rightSpeed;
	
	///////////////////////////////////////////////////  Done  ///////////////////////////////////////////////////

	position.y = terrain->GetY(position.x, position.z) + MovementConstants::CAMERA_HEIGHT_OFFSET;

	if (position.x < 35)
		position.x = 35;
	if (position.x > GameConstants::WORLD_SIZE - 35)
		position.x = GameConstants::WORLD_SIZE - 35;
	if (position.z < 35)
		position.z = 35;
	if (position.z > GameConstants::WORLD_SIZE - 35)
		position.z = GameConstants::WORLD_SIZE - 35;

	camera->SetRotation(rotation);
	camera->SetPosition(position);

	return true;
}

bool GameLogic::UpdatePhysicsObjects(double frameTime, vector<GameObject*>& gameObjects, XMFLOAT3 cannonRotation)
{

	/*PhysicsObject* copy = nullptr;*/
	//for (auto go : gameObjects)
	//{
	//	if (go->GetId() == ObjectTypes::PHYSICS)
	//	{
	//		copy = (PhysicsObject*)go;
	//	}
	//}

	//PhysicsObject* newObj = new PhysicsObject(*copy);
	//newObj->SetPosition(GameConstants::CANNONBALL_START_POS);

	//newObj->SetAcceleration(XMFLOAT3(0, 0, 1));
	//newObj->SetVelocity(XMFLOAT3(0, 55, 55));
	//newObj->WakePhysics();

	//gameObjects.push_back(newObj);

	if (Input->SpaceClicked())
	{
		bool found = false;
		PhysicsObject* copy = nullptr;
		for (auto go : gameObjects)
		{
			if (go->GetId() == ObjectTypes::PHYSICS)
			{
				copy = (PhysicsObject*)go;

				if (!((PhysicsObject*)go)->IsAlive() && !((PhysicsObject*)go)->IsUsed())
				{
					((PhysicsObject*)go)->SetAcceleration(XMFLOAT3(0, 0, 1));
					((PhysicsObject*)go)->SetVelocity(XMFLOAT3(0, 55, 55));
					((PhysicsObject*)go)->WakePhysics();
					found = true;
					break;
				}
			}
		}

		if (!found)
		{
			PhysicsObject* newObj = new PhysicsObject(*copy);
			newObj->SetPosition(GameConstants::CANNONBALL_START_POS);

			newObj->SetAcceleration(XMFLOAT3(0, 0, 1));
			newObj->SetVelocity(XMFLOAT3(0, 55, 55));
			newObj->WakePhysics();

			gameObjects.push_back(newObj);
		}
	}

	for (auto go : gameObjects)
	{
		if (go->GetId() == ObjectTypes::PHYSICS)
		{
			if (((PhysicsObject*)go)->IsAlive())
			{
				if (go->GetPosition().y < 10.0f)
					((PhysicsObject*)go)->KillPhysics();

				
				PhysicsObject* po = (PhysicsObject*)go;
				float timeInSeconds = float(frameTime / 1000);
				
				XMFLOAT3 acc = po->GetAcceleration();
				XMFLOAT3 pos = po->GetPosition();
				XMFLOAT3 vel = po->GetVelocity();
				float mass = po->GetMass();

				pos.x = pos.x + vel.x*timeInSeconds;
				/*pos.y = pos.y + vel.y*timeInSeconds;*/
				pos.y = pos.y + vel.y * timeInSeconds - 0.5f * PhysicsConstants::GRAVITY.y * pow(timeInSeconds,2);
				pos.z = pos.z + vel.z*timeInSeconds;

				vel.x = vel.x + PhysicsConstants::GRAVITY.x * timeInSeconds;
				vel.y = vel.y + PhysicsConstants::GRAVITY.y * timeInSeconds;
				vel.z = vel.z + PhysicsConstants::GRAVITY.z * timeInSeconds;

				po->SetPosition(pos);
				po->SetVelocity(vel);


				//Perform physics calculations here
				//http://stackoverflow.com/questions/25065676/a-c-function-to-calculate-and-sample-the-trajectory-of-a-projectile-in-3d-spac
				//http://www.physics.buffalo.edu/phy410-505-2008/chapter2/ch2-lec1.pdf
				//http://www.splung.com/content/sid/2/page/projectiles
				//http://www.ingvet.kau.se/juerfuch/kurs/amek/prst/06_simu.pdf
				//http://wps.aw.com/wps/media/objects/877/898586/topics/topic01.pdf

				//Vxz = v0xz * cos(Alpha) * t
				//Vy = v0 * sin(Alpha) - g * t


				//Ta fram ny hastighet genom v = v0 + a ??t .
				//Ta fram en ny position genom s = s0 + v0 ??t


				po->Update(frameTime);
			}
		}
	}

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

bool GameLogic::UpdateCannon(GameObject* cannon)
{
	if (Input->UpArrowDown())
	{
		XMFLOAT3 cannonRotation = cannon->GetRotation();

		if (cannonRotation.x > 315)
			cannonRotation.x -= GameConstants::CANNON_PITCH_SPEED;

		cannon->SetRotation(cannonRotation);
	}
	else if (Input->DownArrowDown())
	{
		XMFLOAT3 cannonRotation = cannon->GetRotation();

		if (cannonRotation.x < 359 && cannonRotation.x >= 315)
			cannonRotation.x += GameConstants::CANNON_PITCH_SPEED;

		cannon->SetRotation(cannonRotation);
	}

	return true;
}
