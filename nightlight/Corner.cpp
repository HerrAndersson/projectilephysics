#include "Corner.h"


Corner::Corner(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY)
	: GameObject(id, position, rotation, renderObject, coordX, coordY)
{

}

Corner::~Corner()
{
}
