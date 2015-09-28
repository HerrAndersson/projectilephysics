#include "LevelParser.h"

LevelParser::LevelParser(AssetManager* assetManager)
{
	if (assetManager == nullptr)
	{
		OutputDebugString("Error in LevelParser constructor: assetManager == nullptr");
	}
	else
	{
		this->assetManager = assetManager;
	}
	assetUtility::fileToStrings("Assets/levels.txt", levelNames);
	assetUtility::fileToStrings("Assets/gameObjectTypes.txt", gameObjectTypes);
}

LevelParser::~LevelParser()
{

}

template<typename T>
void bindActuators(vector<T*> gameObjects, vector<Door*> doors, vector<Lever*> levers)
{
	for (T* t : gameObjects)
	{
		bool foundCoupling = false;
		std::string activatesName = t->getActivatesName();
		for (int i = 0; i < (signed)doors.size() && !foundCoupling; i++)
			if (doors.at(i)->getActivationName() == activatesName)
			{
				t->setActivatesDoor(doors.at(i));
				foundCoupling = true;
			}
		for (int i = 0; i < (signed)levers.size() && !foundCoupling; i++)
			if (levers.at(i)->getActivationName() == activatesName)
			{
				t->setActivatesLever(levers.at(i));
				foundCoupling = true;
			}
	}
}

Level* LevelParser::LoadLevel(int levelID, std::vector<Enemy*> &enemies, Character &character, Character &grandpa, Sounds* sounds)
{
	//TODO: Handle button reading from menu level

	Level* level = nullptr;

	if (levelID <= -1 || levelID > (signed)levelNames.size() - 1)
	{
		OutputDebugString("Error on LoadLevel: levelID out of bounds.");
		return level;
	}

	level = new Level(levelID);

	std::string pathToLevel = "Assets/Levels/" + levelNames.at(levelID);
	std::vector<std::string> unparsedLevel;
	assetUtility::fileToStrings(pathToLevel, unparsedLevel);

	if (levelNames.at(levelID) == "levelend.lvl")
	{
		isEnd = true;
	}

	std::vector<std::string> unparsedLine;
	for (int i = 0; i < (int)(unparsedLevel.size() - 1); i++)
	{
		try
		{
			unparsedLine.clear();
			assetUtility::splitStringToVector(unparsedLevel[i], unparsedLine, ",");

			int gameObjectTypeRef = std::stoi(unparsedLine.at(1));
			std::string gameObjectType = gameObjectTypes.at(gameObjectTypeRef);
			
			if (gameObjectType.find("small") != std::string::npos || gameObjectType.find("average") != std::string::npos || gameObjectType.find("large") != std::string::npos)
			{
				int i = 0;
				int renderObjectRef = std::stoi(unparsedLine.at(i++));
				int gameObjectTypeRef = std::stoi(unparsedLine.at(i++));
				float rotation;
				XMFLOAT3 position;
				position.x = std::stof(unparsedLine.at(i++)) - TILE_SIZE / 2;
				position.y = std::stof(unparsedLine.at(i++));
				position.z = std::stof(unparsedLine.at(i++)) - TILE_SIZE / 2;
				rotation = std::stof(unparsedLine.at(i++));
				int tileCoordX = std::stoi(unparsedLine.at(i++));
				int tileCoordY = std::stoi(unparsedLine.at(i++));
				std::string enemyType = std::string(unparsedLine.at(i++));

				enemies.push_back(new Enemy(gameObjectTypeRef, position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, enemyType));
			}
			else if (gameObjectType.find("button") != std::string::npos)
			{
				int i = 0;
				int renderObjectRef = std::stoi(unparsedLine.at(i++));
				int gameObjectTypeRef = std::stoi(unparsedLine.at(i++));
				float rotation;
				XMFLOAT3 position;
				position.x = std::stof(unparsedLine.at(i++)) - TILE_SIZE / 2;
				position.y = std::stof(unparsedLine.at(i++));
				position.z = std::stof(unparsedLine.at(i++)) - TILE_SIZE / 2;
				rotation = std::stof(unparsedLine.at(i++));
				int tileCoordX = std::stoi(unparsedLine.at(i++));
				int tileCoordY = std::stoi(unparsedLine.at(i++));



				std::string buttonType = unparsedLine.at(i++);
				int buttonWidth = stoi(unparsedLine.at(i++));
				Button* button = new Button(gameObjectTypeRef, position, rotation, assetManager->GetRenderObject(renderObjectRef), tileCoordX, tileCoordY, buttonType, buttonWidth);
				
				for (int x = tileCoordX; x < tileCoordX + buttonWidth; x++)
				{
					Tile* tile = level->getTile(x, tileCoordY);
					if (tile == nullptr)
					{
						tile = new Tile();
						level->setTile(tile, x, tileCoordY);
					}
					if (x == tileCoordX)
						tile->AddGameObjectManually(button);
					tile->setButton(button);
				}
			}

			else
			{
				int tileCoordX = std::stoi(unparsedLine.at(6));
				int tileCoordY = std::stoi(unparsedLine.at(7));
				Tile* tile = level->getTile(tileCoordX, tileCoordY);
				if (tile == nullptr)
				{
					tile = new Tile();
					level->setTile(tile, tileCoordX, tileCoordY);
				}
				tile->createGameObjectFromUnparsedData(assetManager, &gameObjectTypes, unparsedLine, sounds);
			}
		}
		catch (...)
		{
			cout << "Error in LevelParser::LoadLevel: " + unparsedLevel[i] + " is not a valid gameObject.\n";
			delete level;
			return nullptr;
		}
	}

	vector<Door*> doors;
	vector<Lever*> levers;
	vector<PressurePlate*> pressurePlates;
	vector<Container*> containers;
	vector<Button*> button;
	Coord startDoor = Coord();

	vector<vector<Tile*>>* tileGrid = level->getTileGrid();
	for (int x = 0; x < (signed)tileGrid->size(); x++){
		for (int y = 0; y < (signed)tileGrid->at(x).size(); y++) 
		{
			Tile* tile = tileGrid->at(x).at(y);
			if (tile != nullptr) {
				Door* tileDoor = tile->getDoor();
				Lever* tileLever = tile->getLever();
				PressurePlate* tilePressurePlate = tile->getPressurePlate();
				Container* tileShadowContainer = tile->getShadowContainer();
				Button* tileButton = tile->getButton();

				if (tileDoor != nullptr)
				{
					if (tileDoor->getDoorType() == Door::DoorTypes::START_DOOR) 
					{
						startDoor = Coord(x, y);
						level->setStartDoor(startDoor);
					}
					if (tileDoor->getDoorType() == Door::DoorTypes::END_DOOR)
						level->setEndDoor(Coord(x, y));
					doors.push_back(tileDoor);
				}
				if (tileLever != nullptr)
				{
					levers.push_back(tileLever);
				}
				if (tilePressurePlate != nullptr)
				{
					pressurePlates.push_back(tilePressurePlate);
				}
				if (tileShadowContainer != nullptr)
				{
					containers.push_back(tileShadowContainer);
				}
				if (tileButton != nullptr)
				{
					button.push_back(tileButton);
				}
			}
		}
	}

	bindActuators(containers, doors, levers);
	bindActuators(pressurePlates, doors, levers);
	bindActuators(levers, doors, levers);


	if (startDoor.x != -1 && startDoor.y != -1) {
		character.SetTilePosition(startDoor);
	}
	level->setPlayerPosition(character.GetPosition());

	level->updateGameObjets();
	
	LightObject* light1 = new LightObject();
	LightObject* light2 = new LightObject();

	if (levelID == 0) {
		light1->setPosition(0, 0, 0);
		light1->setDiffuseColor(0.0f, 0.0f, 0.0f, 0.0f);
		light2->setPosition(-8.0f, -2.0f, -5.5f);
		light2->setDiffuseColor(0.55f, 0.45f, 0.2f, 1.0f);
	}
	else {
		light1->setPosition(-level->getEndDoor().x - 0.5f, -2.6f, -level->getEndDoor().y - 0.5f);
		light1->setDiffuseColor(0.95f, 0.1f, 0.2f, 1.0f);
		light2->setPosition(-7.0f, -2.0f, -5.5f);
		light2->setDiffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
	}

	level->AddLight(light1);
	level->AddLight(light2);

	return level;
}