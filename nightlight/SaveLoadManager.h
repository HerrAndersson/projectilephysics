#pragma once
#include <string>
#include <fstream>
#include <iostream>

class SaveLoadManager
{
public:
	SaveLoadManager();
	~SaveLoadManager();

	bool Load(std::string filePath, int &levelNr);
	bool Save(std::string filePath, int levelNr);
};

