#pragma once
#include "GameObject.h"
class MovableObject : public GameObject 
{
private:

public:

	MovableObject(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY);
	~MovableObject();
};

