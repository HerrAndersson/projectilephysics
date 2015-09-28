#pragma once
#include "GameObject.h"
class Corner : public GameObject 
{
private:

public:
	Corner(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY);
	~Corner ( );
};

