#include "PhysicsObject.h"


PhysicsObject::PhysicsObject(int id, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT3 rotation)
			 : GameObject(id, renderObject, position, scale, rotation)
{
	isAlive = false;
	timeAlive = 0;
	velocity = XMFLOAT3(0, 0, 0);
	acceleration = XMFLOAT3(0, 0, 0);
}

PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::Update(double frameTime)
{
	//SetPosition(XMFLOAT3(512 + sin(float(timeAlive/1000.0f))* 100, 200 + cos(float(timeAlive / 1000.0f)) * 100, position.z));
	timeAlive += frameTime;
}

void PhysicsObject::WakePhysics()
{
	isAlive = true;
}

void PhysicsObject::KillPhysics()
{
	isAlive = false;
}

bool PhysicsObject::IsAlive()
{
	return isAlive;
}

void PhysicsObject::SetVelocity(XMFLOAT3 velocity)
{
	this->velocity = velocity;
}
void PhysicsObject::SetAcceleration(XMFLOAT3 acceleration)
{
	this->acceleration = acceleration;
}

XMFLOAT3 PhysicsObject::GetVelocity()
{
	return velocity;
}
XMFLOAT3 PhysicsObject::GetAcceleration()
{
	return acceleration;
}

float PhysicsObject::GetTimeAlive()
{
	return timeAlive;
}
