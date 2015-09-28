#include "Door.h"

Door::Door(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, bool isOpen, int doorType, std::string activationName, Sounds* sounds)
	: GameObject(id, position, rotation, renderObject, coordX, coordY)
{
	this->isOpen = isOpen;
	this->doorType = doorType;
	this->activationName = activationName;
	this->sounds = sounds;
}


Door::~Door()
{

}

void Door::setIsOpen(bool val) {
	isOpen = val;

	if (isOpen) {
		position.y = 1.9f;
		sounds->doorOpen.setPosition(YSE::Vec(position.x, 0, position.z));
		if (doorType == 2){
			sounds->endDoorOpen.play();
		}
		else{
			sounds->doorOpen.play();
		}
	} else {
		position.y = 0.0f;
	}
}
