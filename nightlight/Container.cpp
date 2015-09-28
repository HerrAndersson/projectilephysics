#include "Container.h"


Container::Container(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string activatesName, Sounds* sounds)
	: GameObject(id, position, rotation, renderObject, coordX, coordY) {
	this->activatesName = activatesName;
	this->sounds = sounds;
}


Container::~Container ( ) {
}

void Container::ActivateContainer() {
	isActivated = true;
	colorModifier = XMFLOAT3(0.2f, 0.2f, 0.0f);

	sounds->shadowContainerActivated.play();

	if (activatesLever != nullptr) {
		activatesLever->setIsPowered(true);
	}
	if (activatesDoor != nullptr) {
		activatesDoor->setIsOpen(true);
	}
}