#include "PressurePlate.h"


PressurePlate::PressurePlate(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string activatesName)
	: GameObject(id, position, rotation, renderObject, coordX, coordY)
{
	this->activatesName = activatesName;
}


PressurePlate::~PressurePlate()
{
}

void PressurePlate::ActivatePressurePlate() {
	isActivated = true;
	colorModifier = XMFLOAT3(0.1f, 0.1f, 0.2f);

	if (activatesLever != nullptr) {
		activatesLever->setIsPowered(true);
	}
	if (activatesDoor != nullptr) {
		activatesDoor->setIsOpen(true);
	}
}

void PressurePlate::DeactivatePressurePlate() {
	isActivated = false;
	colorModifier = XMFLOAT3(0, 0, 0);
	
	if (activatesLever != nullptr) {
		activatesLever->DeactivateLever();
		activatesLever->setIsPowered(false);
	}
	if (activatesDoor != nullptr) {
		activatesDoor->setIsOpen(false);
	}
}
