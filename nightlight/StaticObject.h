#pragma once
#include "GameObject.h"
class StaticObject : public GameObject 
{
private:

public:

	StaticObject(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY);
	~StaticObject ( );
};

