#pragma once
#include "GameObject.h"

class PhysicsObject : public GameObject
{
private:

	XMFLOAT3 velocity;
	float mass;
	float angle;

	bool isAlive;
	bool used;
	double timeAlive;
	float radius;
	float crossSectionalArea;

public:

	PhysicsObject(int id, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT3 rotation, float density, float radius);
	PhysicsObject(const PhysicsObject& other);
	virtual ~PhysicsObject();

	void Update(double frameTime);

	void WakePhysics();
	void KillPhysics();

	bool IsAlive();
	bool IsUsed();

	void SetVelocity(XMFLOAT3 velocity);

	XMFLOAT3 GetVelocity();
	float GetMass();
	double GetTimeAlive();
	float GetCrossSectionalArea();
	float GetRadius();

};

