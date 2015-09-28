#include "Level.h"


Level::Level(int levelNr)
{
	this->levelNr = levelNr;
	startDoor = Coord();
	endDoor = Coord();
}

Level::~Level()
{
	for (auto go : gameObjects) delete go;
	gameObjects.clear();

	for (auto l : lights) delete l;
	lights.clear();

	for (auto tg : tileGrid)
	{
		for (auto t : tg) delete t;

		tg.clear();
	}
	tileGrid.clear();
}

void Level::updateGameObjets()
{
	gameObjects.clear();
	for (int x = 0; x < (signed)tileGrid.size(); x++)
		for (int y = 0; y < (signed)tileGrid[x].size(); y++)
		{
			try
			{
				if (tileGrid[x][y] != nullptr){
					std::vector<GameObject*>* tileGameObjects = tileGrid[x][y]->getAllGameObjects();
					for (int i = 0; i < (signed)tileGameObjects->size(); i++)
					{
						gameObjects.push_back(tileGameObjects->at(i));
					}
				}
			}
			catch (...) {}
		}
}

Tile* Level::getTile (int x, int y) 
{
	Tile* tile = nullptr;
	if (x >= 0 && y >= 0)
	{
		try 
		{
			tile = tileGrid.at(x).at(y);
		}
		catch (...) {}
	}
	return tile;
}

Tile* Level::getTile(Coord coord) {
	return getTile(coord.x, coord.y);
}

//If there is already tile at x,y it will be deleted.
void Level::setTile(Tile* tile, int x, int y)
{
	try
	{
		delete tileGrid.at(x).at(y);
	}
	catch (...) {}

	if (x > (int)(tileGrid.size() - 1)) 
	{
		tileGrid.resize(x + 1);
	}
	if (y > (int)(tileGrid.at(x).size() - 1)) 
	{
		tileGrid.at(x).resize(y + 1);
	}
	tileGrid.at(x).at(y) = tile;
}

int Level::sizeX()
{
	return tileGrid.size();
}
int Level::sizeY(int x)
{
	return tileGrid.at(x).size();
}

bool Level::withinBounds(int x, int y)
{
	bool result = false;
	if (x >= 0 && x < (signed)tileGrid.size())
	{
		if (y >= 0 && y < (signed)tileGrid.at(x).size())
		{
			result = true;
		}
	}
	return result;
}
