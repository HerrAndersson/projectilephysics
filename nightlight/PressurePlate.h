#pragma once
#include "GameObject.h"
#include "Lever.h"
#include "Door.h"

class PressurePlate :
	public GameObject {
public:

	PressurePlate(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string activatesName);
	~PressurePlate ( );

	std::string getActivatesName() const { return activatesName; }

	Lever* getActivatesLever() const { return activatesLever; }
	void setActivatesLever(Lever* val) { activatesLever = val; }
	Door* getActivatesDoor() const { return activatesDoor; }
	void setActivatesDoor(Door* val) { activatesDoor = val; }
	bool getIsActivated() const { return isActivated; }
	void ActivatePressurePlate();
	void DeactivatePressurePlate();

private:
	std::string activatesName = "";
	Lever* activatesLever = nullptr;
	Door* activatesDoor = nullptr;
	bool isActivated = false;
};

