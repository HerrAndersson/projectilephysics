#pragma once
#include "GameObject.h"
class Floor: public GameObject 
{
private:


public:

	Floor(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY);
	~Floor();
};

