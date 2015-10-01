#pragma once
#include "GameObject.h"

class PhysicsObject : public GameObject
{
private:

	const XMFLOAT3 GRAVITY				= XMFLOAT3(0, -9.82114f, 0); //Karlskrona
	//const double AIR_RESISTANCE			= 

	XMFLOAT3 velocity;
	XMFLOAT3 acceleration;

	bool isAlive;
	double timeAlive;

public:

	PhysicsObject(int id, RenderObject* renderObject, XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT3 rotation);
	virtual ~PhysicsObject();

	void Update(double gameTime);

	void WakePhysics();
	void KillPhysics();

	void SetVelocity(XMFLOAT3 velocity);
	void SetAcceleration(XMFLOAT3 acceleration);

	XMFLOAT3 GetVelocity();
	XMFLOAT3 GetAcceleration();

};

