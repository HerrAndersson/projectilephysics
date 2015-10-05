#include "GameLogic.h"

GameLogic::GameLogic(InputManager* Input)
{
	this->Input = Input;
	cannonLaunchSpeed = 80;
}

GameLogic::~GameLogic()
{

}

int GameLogic::GetLaunchSpeed()
{
	return cannonLaunchSpeed;
}

bool GameLogic::Update(double frameTime, double gameTime, vector<PhysicsObject*>& projectiles, Camera* camera, GameObject* skySphere, Terrain* terrain, GameObject* cannon)
{
	Input->HandleMouse();

	if (!UpdateCamera(frameTime, camera, terrain))
		return false;

	if (!UpdateCannon(cannon))
		return false;

	if (!UpdatePhysicsObjects(frameTime, projectiles, cannon->GetRotation()))
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

bool GameLogic::UpdatePhysicsObjects(double frameTime, vector<PhysicsObject*>& projectiles, XMFLOAT3 cannonRotation)
{

	PhysicsObject* copy2 = nullptr;
	for (auto p : projectiles)
	{
		if (p->GetId() == ObjectTypes::PHYSICS)
		{
			copy2 = p;
		}
	}

	PhysicsObject* newObj = new PhysicsObject(*copy2);
	newObj->SetPosition(GameConstants::CANNONBALL_START_POS);

	newObj->SetAcceleration(XMFLOAT3(0, 0, 100));

	float b = sin(XMConvertToRadians(360 - cannonRotation.x));
	float c = cos(XMConvertToRadians(360 - cannonRotation.x));

	float velY = cannonLaunchSpeed * b;
	float velZ = cannonLaunchSpeed * c;

	newObj->SetVelocity(XMFLOAT3(0, velY, velZ));
	newObj->WakePhysics();
	projectiles.push_back(newObj);

	if (Input->SpaceClicked())
	{
		bool found = false;
		PhysicsObject* copy = nullptr;

		for (auto p : projectiles)
		{
			if (p->GetId() == ObjectTypes::PHYSICS)
			{
				copy = p;

				if (!p->IsAlive() && !p->IsUsed())
				{
					p->SetAcceleration(XMFLOAT3(0, 0, 100));

					float b = sin(XMConvertToRadians(360 - cannonRotation.x));
					float c = cos(XMConvertToRadians(360 - cannonRotation.x));

					float velY = cannonLaunchSpeed * b;
					float velZ = cannonLaunchSpeed * c;

					p->SetVelocity(XMFLOAT3(0, velY, velZ));
					p->WakePhysics();
					found = true;
					break;
				}
			}
		}

		if (!found)
		{
			PhysicsObject* newObj = new PhysicsObject(*copy);
			newObj->SetPosition(GameConstants::CANNONBALL_START_POS);

			newObj->SetAcceleration(XMFLOAT3(0, 0, 100));

			float b = sin(XMConvertToRadians(360 - cannonRotation.x));
			float c = cos(XMConvertToRadians(360 - cannonRotation.x));

			float velY = cannonLaunchSpeed * b;
			float velZ = cannonLaunchSpeed * c;

			newObj->SetVelocity(XMFLOAT3(0, velY, velZ));
			newObj->WakePhysics();

			projectiles.push_back(newObj);
		}
	}

	for (auto p : projectiles)
	{
		if (p->GetId() == ObjectTypes::PHYSICS)
		{
			if (p->IsAlive())
			{
				if (p->GetPosition().y < 10.0f)
					p->KillPhysics();

				
				float timeStep = float(frameTime / 100);
				
				XMFLOAT3 acc = p->GetAcceleration();
				XMFLOAT3 pos = p->GetPosition();
				XMFLOAT3 vel = p->GetVelocity();
				float mass = p->GetMass();
				float alpha = atan((pos.y - GameConstants::CANNONBALL_START_POS.y) / (pos.z - GameConstants::CANNONBALL_START_POS.z));


				pos.x = pos.x + vel.x*timeStep;


				/*pos.y = pos.y + vel.y*timeInSeconds;*/
				pos.y = pos.y + vel.y * timeStep - 0.5f * PhysicsConstants::GRAVITY.y * pow(timeStep,2);
				//float cannonRotationY = XMConvertToRadians(cannonRotation.y);
				//pos.y = pos.y * XMConvertToDegrees(cos(a)) * timeInSeconds;

				pos.z = pos.z + vel.z*timeStep;

				//Sideways gravity or air resistance
				//pos.z = pos.z + vel.z * timeStep - 0.5f * DRAG * pow(timeStep, 2);

				vel.x = vel.x + PhysicsConstants::GRAVITY.x * timeStep;
				vel.y = vel.y + PhysicsConstants::GRAVITY.y * timeStep;
				vel.z = vel.z + PhysicsConstants::GRAVITY.z * timeStep;

				p->SetPosition(pos);
				p->SetVelocity(vel);


				//Perform physics calculations here
				//http://stackoverflow.com/questions/25065676/a-c-function-to-calculate-and-sample-the-trajectory-of-a-projectile-in-3d-spac
				//http://www.physics.buffalo.edu/phy410-505-2008/chapter2/ch2-lec1.pdf
				//http://www.splung.com/content/sid/2/page/projectiles
				//http://www.ingvet.kau.se/juerfuch/kurs/amek/prst/06_simu.pdf
				//http://wps.aw.com/wps/media/objects/877/898586/topics/topic01.pdf
				//http://www.codeproject.com/Articles/19107/Flight-of-a-projectile

				//Vxz = v0xz * cos(Alpha) * t
				//Vy = v0 * sin(Alpha) - g * t


				//Ta fram ny hastighet genom v = v0 + a ??t .
				//Ta fram en ny position genom s = s0 + v0 ??t


				p->Update(frameTime);
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

		if (cannonRotation.x > 295)
			cannonRotation.x -= GameConstants::CANNON_PITCH_SPEED;

		cannon->SetRotation(cannonRotation);
	}
	else if (Input->DownArrowDown())
	{
		XMFLOAT3 cannonRotation = cannon->GetRotation();

		if (cannonRotation.x < 359)
			cannonRotation.x += GameConstants::CANNON_PITCH_SPEED;

		cannon->SetRotation(cannonRotation);
	}

	if (Input->LeftArrowDown())
	{
		if(cannonLaunchSpeed > 0)
			cannonLaunchSpeed--;
	}
	else if (Input->RightArrowDown())
	{
		if(cannonLaunchSpeed < GameConstants::MAX_LAUNCH_SPEED)
			cannonLaunchSpeed++;
	}

	return true;
}
