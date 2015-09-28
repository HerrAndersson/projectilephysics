#include "GameLogic.h"
#include "Collision.h"

GameLogic::GameLogic(InputManager* Input)
{
	this->Input = Input;
}

GameLogic::~GameLogic()
{

}

bool GameLogic::Update()
{
	return true;
}
