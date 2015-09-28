#pragma once
#include "GameObject.h"
#include "Door.h"

class Lever :
	public GameObject {
public:

	Lever(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, bool isPowered, bool isActivated, std::string activationName, std::string activatesName, Sounds* sounds);
	~Lever();

	std::string getActivationName() const { return activationName; }
	std::string getActivatesName() const { return activatesName; }


	Lever* getActivatesLever() const { return activatesLever; }
	void setActivatesLever(Lever* val) { activatesLever = val; }
	Door* getActivatesDoor() const { return activatesDoor; }
	void setActivatesDoor(Door* val) { activatesDoor = val; }
	bool getIsPowered() const { return isPowered; }
	void setIsPowered(bool val) { isPowered = val; }
	bool getIsActivated() const { return isActivated; }
	void ActivateLever();
	void DeactivateLever();

private:
	bool isPowered;
	bool isActivated;
	std::string activationName = "";
	std::string activatesName = "";
	Lever* activatesLever = nullptr;
	Door* activatesDoor = nullptr;
	Sounds* sounds;
};

