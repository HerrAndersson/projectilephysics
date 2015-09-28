#pragma once
#include "AssetManager.h"
#include "Level.h"
#include "Tile.h"
#include "Character.h"
#include "Enemy.h"

class LevelParser 
{
private:
	AssetManager* assetManager = nullptr;
	std::vector<std::string> levelNames;
	std::vector<std::string> gameObjectTypes;

public:
	LevelParser ( AssetManager* assetManager );
	~LevelParser ( );

	bool isEnd = false;

	Level* LoadLevel(int levelID, std::vector<Enemy*> &enemies, Character &character, Character &grandpa, Sounds* sounds);
	int GetLevelCount(){ return levelNames.size(); }
};
