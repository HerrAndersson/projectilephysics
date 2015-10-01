#include "SaveLoadManager.h"


SaveLoadManager::SaveLoadManager()
{

}


SaveLoadManager::~SaveLoadManager()
{
}

bool SaveLoadManager::Load(std::string filePath, int &levelNr)
{
	std::ifstream file(filePath);
	if (!file.is_open())
		return false;

	std::string line;

	try
	{
		while (getline(file, line))
		{
			levelNr = std::stoi(line);
			return true;
		}
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool SaveLoadManager::Save(std::string filePath, int levelNr)
{
	std::ofstream file(filePath);
	if (!file.is_open())
		return false;

	file.clear();

	file << levelNr;
	file.close();

	return true;
}
