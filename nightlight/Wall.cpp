#include "Wall.h"

Wall::Wall(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY)
	: GameObject(id, position, rotation, renderObject, coordX, coordY)
{

}

Wall::~Wall()
{

}
