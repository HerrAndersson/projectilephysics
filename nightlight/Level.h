#pragma once
#include "Tile.h"
#include "lightObject.h"

using std::vector;

class Level 
{
private:
	std::vector<std::vector<Tile*>> tileGrid;
	std::vector<GameObject*> gameObjects;
	std::vector<LightObject*> lights;
	Coord startDoor;
	Coord endDoor;
	XMFLOAT3 currentPlayerPosition;
	int levelNr = -1;

public:
	Level (int levelNr );
	~Level ( );
	std::vector<GameObject*>* GetGameObjects(){ return &gameObjects; };
	void updateGameObjets();

	Tile* getTile (int x, int y);
	Tile* getTile (Coord coord);
	void setTile(Tile* tile, int x, int y);
	Coord getStartDoorCoord() const { return startDoor; }
	void setStartDoor(Coord val) { startDoor = val; }
	Coord getEndDoor() const { return endDoor; }
	void setEndDoor(Coord val) { endDoor = val; }
	XMFLOAT3 getPlayerPostion() const { return currentPlayerPosition; }
	void setPlayerPosition(XMFLOAT3 PlayerPosition) { currentPlayerPosition = PlayerPosition; }
	int GetLevelNr() const { return levelNr; }
	void AddLight(LightObject* light) { lights.push_back(light); }
	LightObject* GetLightAt(int at) const { return lights.at(at); }
	std::vector<LightObject*> GetLights() const { return lights; }

	vector<vector<Tile*>>* getTileGrid() { return &tileGrid; }

	int sizeX();
	int sizeY(int x);

	bool withinBounds(int x, int y);
};

