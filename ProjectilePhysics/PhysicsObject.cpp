#include "PhysicsObject.h"


PhysicsObject::PhysicsObject(int id, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT3 rotation, float density, float radius)
			 : GameObject(id, renderObject, position, scale, rotation)
{
	isAlive = false;
	used = false;
	timeAlive = 0;
	velocity = XMFLOAT3(0, 0, 0);

	this->radius = radius;
	this->crossSectionalArea = XM_PI * pow(radius, 2);
	float volume = (4 * XM_PI*pow(radius, 3)) / 3;
	this->mass = density * volume;
}

PhysicsObject::PhysicsObject(const PhysicsObject& other) : GameObject(other)			 
{
	isAlive = false;
	used = false;
	timeAlive = 0;
	velocity = XMFLOAT3(0, 0, 0);

	this->mass = other.mass;
	this->radius = other.radius;
	this->crossSectionalArea = other.crossSectionalArea;
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
	used = true;
}

bool PhysicsObject::IsAlive()
{
	return isAlive;
}

bool PhysicsObject::IsUsed()
{
	return used;
}

void PhysicsObject::SetVelocity(XMFLOAT3 velocity)
{
	this->velocity = velocity;
}

XMFLOAT3 PhysicsObject::GetVelocity()
{
	return velocity;
}

double PhysicsObject::GetTimeAlive()
{
	return timeAlive;
}

float PhysicsObject::GetMass()
{
	return mass;
}

float PhysicsObject::GetCrossSectionalArea()
{
	return crossSectionalArea;
}

float PhysicsObject::GetRadius()
{
	return radius;
}
