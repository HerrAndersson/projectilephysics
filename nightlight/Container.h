#pragma once
#include "GameObject.h"
#include "Lever.h"
#include "Door.h"

class Container : public GameObject
{
private:

	std::string activatesName = "";
	Lever* activatesLever = nullptr;
	Door* activatesDoor = nullptr;
	bool isActivated = false;
	Sounds* sounds;

public:
	Container(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string activatesName, Sounds* sounds);
	~Container();

	std::string getActivatesName() const { return activatesName; }

	Lever* getActivatesLever() const { return activatesLever; }
	void setActivatesLever(Lever* val) { activatesLever = val; }
	Door* getActivatesDoor() const { return activatesDoor; }
	void setActivatesDoor(Door* val) { activatesDoor = val; }

	bool GetIsActivated() const { return isActivated; }
	void ActivateContainer();
};

