#include "GameLogic.h"
#include "Collision.h"

GameLogic::GameLogic(InputManager* Input)
{
	this->Input = Input;
}

GameLogic::~GameLogic()
{

}

bool GameLogic::Update(double gameTime, vector<GameObject*> gameObjects, Camera* camera)
{
	((PhysicsObject*)(gameObjects.at(0)))->Update(gameTime);

	if (Input->LeftMouseDown())
	{
		XMFLOAT3 pos = camera->GetPosition();
		camera->SetPosition(pos.x, pos.y + 1, pos.z);
	}
	else if (Input->RightMouse())
	{
		XMFLOAT3 pos = camera->GetPosition();
		camera->SetPosition(pos.x, pos.y - 1, pos.z);
	}

	return true;
}
