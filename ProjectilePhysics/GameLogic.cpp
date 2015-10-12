#include "GameLogic.h"

GameLogic::GameLogic(InputManager* Input)
{
	this->Input = Input;
	cannonLaunchSpeed = GameConstants::INITIAL_LAUNCH_SPEED;
	airResistanceOn = true;
}

GameLogic::~GameLogic()
{

}

float GameLogic::GetLaunchSpeed()
{
	return cannonLaunchSpeed;
}

bool GameLogic::AirResistanceOn()
{
	return airResistanceOn;
}

bool GameLogic::Update(double frameTime, double gameTime, vector<PhysicsObject*>& projectiles, Camera* camera, GameObject* skySphere, Terrain* terrain, GameObject* cannon)
{
	Input->HandleMouse();

	if (!UpdateCamera(frameTime, camera, terrain))
		return false;

	if (!UpdateCannon(cannon))
		return false;

	if (!UpdatePhysicsObjects(frameTime, gameTime, projectiles, cannon->GetRotation(), terrain))
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

bool GameLogic::UpdatePhysicsObjects(double frameTime, double gameTime, vector<PhysicsObject*>& projectiles, XMFLOAT3 cannonRotation, Terrain* terrain)
{

	//if (int(gameTime) % int(frameTime) < 5)
	//{
	//	PhysicsObject* copy2 = nullptr;
	//	for (auto p : projectiles)
	//	{
	//		if (p->GetId() == ObjectTypes::PHYSICS)
	//		{
	//			copy2 = p;
	//		}
	//	}

	//	PhysicsObject* newObj = new PhysicsObject(*copy2);
	//	newObj->SetPosition(GameConstants::CANNONBALL_START_POS);

	//	newObj->SetAcceleration(XMFLOAT3(0, 0, 100));

	//	float b = sin(XMConvertToRadians(360 - cannonRotation.x));
	//	float c = cos(XMConvertToRadians(360 - cannonRotation.x));

	//	float velY = cannonLaunchSpeed * b;
	//	float velZ = cannonLaunchSpeed * c;

	//	newObj->SetVelocity(XMFLOAT3(0, velY, velZ));
	//	newObj->WakePhysics();
	//	projectiles.push_back(newObj);
	//}

	if (Input->RightMouseClicked())
		airResistanceOn = !airResistanceOn;
	

	if (Input->LeftMouseClicked())
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
					p->SetPosition(GameConstants::CANNONBALL_START_POS);
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
				
				float timeStep = float(frameTime / 1000);	
				XMFLOAT3 pos = p->GetPosition();
				XMFLOAT3 vel = p->GetVelocity();
				float mass = p->GetMass();
				float alpha = XMConvertToRadians(360 - cannonRotation.x);

				float airDragForce = 0.5f * PhysicsConstants::DRAG_COEFF_SPHERE * PhysicsConstants::AIR_DENSITY*p->GetCrossSectionalArea();
				float airDragY = -airDragForce * pow(vel.y, 2) * sin(alpha);
				float airDragZ = -(airDragForce / p->GetMass()) * pow(vel.z, 2) * cos(alpha);

				if (airResistanceOn)
				{
					pos.y = pos.y + MeterToUnits(vel.y * timeStep - 0.5f * (PhysicsConstants::GRAVITY.y + airDragY) * pow(timeStep, 2));
					pos.z = pos.z + MeterToUnits(vel.z * timeStep - 0.5f * (airDragZ) * pow(timeStep, 2));

					vel.x = vel.x + PhysicsConstants::GRAVITY.x * timeStep;
					vel.y = vel.y + (PhysicsConstants::GRAVITY.y + airDragY) * timeStep;
					vel.z = vel.z + (PhysicsConstants::GRAVITY.z + airDragZ) * timeStep;
				}
				else
				{
					pos.y = pos.y + MeterToUnits(vel.y * timeStep - 0.5f * PhysicsConstants::GRAVITY.y * pow(timeStep, 2));
					pos.z = pos.z + MeterToUnits(vel.z * timeStep);

					vel.x = vel.x + PhysicsConstants::GRAVITY.x * timeStep;
					vel.y = vel.y + PhysicsConstants::GRAVITY.y * timeStep;
					vel.z = vel.z + PhysicsConstants::GRAVITY.z * timeStep;
				}

				if (pos.y <= terrain->GetY(pos.x, pos.z) + MeterToUnits(p->GetRadius()))
				{
	
					pos.y = terrain->GetY(pos.x, pos.z) + MeterToUnits(p->GetRadius() + 0.2f);

					//vel.y *= -0.2f - float((rand() % 10) / 10.0f);
					//vel.z *= 0.2f - float((rand() % 10) / 10.0f);

					//GER INTE NORMALEN FÖR QUADEN UTAN FÖR VERTEXPUNKTEN! GÖR LIKNANDE SOM FÖR GETY()
					XMVECTOR normal = XMLoadFloat3(&terrain->GetNormalAt((int)pos.x, (int)pos.z));
					XMVECTOR velVec = XMLoadFloat3(&vel);

					XMVECTOR responseForceVec = XMVector3Dot(velVec, -normal) * normal;
					XMFLOAT3 responseForce;
					XMStoreFloat3(&responseForce, responseForceVec);

					vel.x += responseForce.x;
					vel.y += responseForce.y;
					vel.z += responseForce.z;

					velVec = XMVectorSet(vel.x, vel.y, vel.z, 1.0f);
					velVec = XMVector3Normalize(velVec);
					XMFLOAT3 v;
					XMStoreFloat3(&v, velVec);

					if(abs(v.z) + abs(v.y) < 1.0f)
						p->KillPhysics();

					//http://gamedevelopment.tutsplus.com/tutorials/create-custom-2d-physics-engine-aabb-circle-impulse-resolution--gamedev-6331
					//http://stackoverflow.com/questions/9806630/calculating-the-vertex-normals-of-a-quad
				}

				p->SetPosition(pos);
				p->SetVelocity(vel);

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

		if (cannonRotation.x < 358.9)
			cannonRotation.x += GameConstants::CANNON_PITCH_SPEED;

		cannon->SetRotation(cannonRotation);
	}

	if (Input->LeftArrowDown())
	{
		if (cannonLaunchSpeed > GameConstants::MIN_LAUNCH_SPEED)
			cannonLaunchSpeed -= 0.1f;;
	}
	else if (Input->RightArrowDown())
	{
		if (cannonLaunchSpeed < GameConstants::MAX_LAUNCH_SPEED)
			cannonLaunchSpeed += 0.1f;
	}

	return true;
}
