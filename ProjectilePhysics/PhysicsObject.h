#pragma once
#include "GameObject.h"

class PhysicsObject : public GameObject
{
private:

	XMFLOAT3 velocity;
	XMFLOAT3 acceleration;

	bool isAlive;
	double timeAlive;

public:

	PhysicsObject(int id, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT3 rotation);
	virtual ~PhysicsObject();

	void Update(double frameTime);

	void WakePhysics();
	void KillPhysics();

	bool IsAlive();

	void SetVelocity(XMFLOAT3 velocity);
	void SetAcceleration(XMFLOAT3 acceleration);

	XMFLOAT3 GetVelocity();
	XMFLOAT3 GetAcceleration();
	float GetTimeAlive();

};

