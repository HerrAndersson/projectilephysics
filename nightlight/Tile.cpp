#include "Tile.h"


Tile::Tile ( ) 
{
}


Tile::~Tile ( ) 
{

}

void Tile::createGameObjectFromUnparsedData(AssetManager* assetManager, std::vector<std::string>* gameObjectTypes, std::vector<std::string> unparsedData, Sounds* sounds)
{
	int i = 0;
	int renderObjectRef = std::stoi(unparsedData.at(i++));

	int gameObjectTypeRef = std::stoi(unparsedData.at(i++));

	float rotation;
	XMFLOAT3 position;
	position.x = std::stof(unparsedData.at(i++)) - TILE_SIZE / 2;
	//position.x = std::stof(unparsedData.at(i++));
	position.y = std::stof(unparsedData.at(i++));
	position.z = std::stof(unparsedData.at(i++)) - TILE_SIZE / 2;
	//position.z = std::stof(unparsedData.at(i++));
	rotation = std::stof(unparsedData.at(i++));

	int tileCoordX = std::stoi(unparsedData.at(i++));
	int tileCoordY = std::stoi(unparsedData.at(i++));

	tileCoord = XMINT2(tileCoordX, tileCoordY);

	std::string gameObjectType = gameObjectTypes->at(gameObjectTypeRef);
	if (gameObjectType == "floor") 
	{
		floorTile = new Floor(gameObjectTypeRef, position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY);
		gameObjects.push_back(floorTile);
	}
	else if (gameObjectType == "wall") 
	{
		wall = new Wall(gameObjectTypeRef, position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY);
		gameObjects.push_back(wall);
	}
	else if (gameObjectType == "corner") 
	{
		corner = new Corner(gameObjectTypeRef, position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY);
		gameObjects.push_back(corner);
	}
	else if (gameObjectType == "door") 
	{
		bool isOpen = std::stoi(unparsedData.at(i++)) == 1;
		int doorType = std::stoi(unparsedData.at(i++));
		std::string activationName = unparsedData.at(i++);
		door = new Door(gameObjectTypeRef, position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, isOpen, doorType, activationName, sounds);
		gameObjects.push_back(door);
	}
	else if (gameObjectType == "static") 
	{
		staticObject = new StaticObject(gameObjectTypeRef, position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY);
		gameObjects.push_back(staticObject);
	}
	else if (gameObjectType == "movable") 
	{
		movableObject = new MovableObject(gameObjectTypeRef, position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY);
		gameObjects.push_back(movableObject);
	}
	else if (gameObjectType == "lever") 
	{
		bool isPowered = std::stoi(unparsedData.at(i++)) == 1;
		bool isActivated = std::stoi(unparsedData.at(i++)) == 1;
		std::string activationName = unparsedData.at(i++);
		std::string activates = unparsedData.at(i++);
		lever = new Lever(gameObjectTypeRef, position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, isPowered, isActivated, activationName, activates, sounds);
		gameObjects.push_back(lever);
	}
	else if (gameObjectType == "pressure") 
	{
		std::string activates = unparsedData.at(i++);
		pressurePlate = new PressurePlate(gameObjectTypeRef, position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, activates);
		gameObjects.push_back(pressurePlate);
	}
	else if (gameObjectType == "container") 
	{
		std::string activates = unparsedData.at(i++);
		shadowContainer = new Container(gameObjectTypeRef, position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, activates, sounds);
		gameObjects.push_back(shadowContainer);
	}
}

void Tile::setButton(Button* button) 
{
	this->button = button;
}

void Tile::setMovableObject(MovableObject* movable)
{
	if (this->movableObject)
		for (int i = 0; i < (signed)gameObjects.size(); i++)
			if (gameObjects.at(i) == movableObject)
				gameObjects.erase(gameObjects.begin() + i);
	
	if (movable)
		gameObjects.push_back(movable);

	this->movableObject = movable;
}

bool Tile::IsWalkable(bool moveObjectMode, GameObject* selectedObject)
{
	if (this == nullptr)
		return false;

	if (shadowContainer || staticObject || (!floorTile && !pressurePlate))
		return false;

	if (movableObject)
		if (moveObjectMode && selectedObject == movableObject)
			return true;
		else
			return false;

	return true;
}

void Tile::AddGameObjectManually(GameObject* gameObject)
{
	gameObjects.push_back(gameObject);
}

bool Tile::IsWalkableAI()
{
	if (this == nullptr)
		return false;

	if (!floorTile && !pressurePlate)
		return false;

	if (movableObject)
		return false;

	if (pressurePlate)
		return true;
	else if (button)
		return true;
	else if (lever)
		return false;

	if(shadowContainer)
		return false;
	else if(staticObject)
		return false;

	if (door && !door->getIsOpen())
		return false;

	return true;
}

bool Tile::SeeThrough()
{
	if (this == nullptr)
		return false;

	if (!floorTile)
		return false;

	if (shadowContainer || staticObject)
		return false;
	
	if (door && !door->getIsOpen())
		return false;
	
	return true;
}
