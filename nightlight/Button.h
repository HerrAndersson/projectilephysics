#pragma once
#include "GameObject.h"

class Button : public GameObject
{
public:
	enum ButtonTypes {START,CONTINUE,EXIT};

	Button(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY, std::string buttonType, int buttonWidth);
	~Button();

	int getButtonType()  const { return buttonType; }
	int getButtonWidth() const { return buttonWidth; }

private:
	int buttonType = -1;
	bool isActivated = false;
	int buttonWidth;
};