#include "PhysicsObject.h"


PhysicsObject::PhysicsObject(int id, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT3 rotation, float mass)
			 : GameObject(id, renderObject, position, scale, rotation)
{
	isAlive = false;
	timeAlive = 0;
	this->mass = mass;
	velocity = XMFLOAT3(0, 0, 0);
	acceleration = XMFLOAT3(0, 0, 0);
}

PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::Update(double frameTime)
{
	timeAlive += frameTime;
}

void PhysicsObject::WakePhysics()
{
	timeAlive = 0;
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

void PhysicsObject::SetAngle(float angle)
{
	this->angle = angle;
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

float PhysicsObject::GetAngle()
{
	return angle;
}
