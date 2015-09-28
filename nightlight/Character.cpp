#include "Character.h"


Character::Character(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY)
	: GameObject(id, position, rotation, renderObject, coordX, coordY)
{
	SetUpAnimation(renderObject, 1.8f);
	SetPrimaryAnimation(0);
	PlayAnimation(0);
}

Character::~Character ( ) 
{
}


void Character::UpdateCharacterAnimation()
{
	if (currentAnim == primaryAnim)
	{
		UpdatePrimaryAnimation();
	}
	else
	{
		UpdateSecondaryAnimation();
	}
}

void Character::UpdatePrimaryAnimation()
{
	if(moved)
	{
		frame += 0.1f;
		if (frame > (framelength * 2))
			frame = 0;

		if (frame < (framelength * 2))
		{
			Weights.x = 0;
			Weights.y = 0;
			Weights.z = (-frame + (framelength * 2)) / framelength;
			Weights.w = (frame - (framelength)) / framelength;
		}
		if (frame < (framelength))
		{
			Weights.x = 0;
			Weights.y = (-frame + (framelength)) / framelength;
			Weights.z = frame / framelength;
			Weights.w = 0;
		}
	}
	else{
		frame = 0;
		Weights = { 1, 0, 0, 0 };
	}
}

void Character::UpdateSecondaryAnimation()
{
	frame += 0.1f;
	if (frame > (framelength * 3))
		PlayAnimation(primaryAnim);

	if (frame < (framelength * 3))
	{
		Weights.x = 0;
		Weights.y = 0;
		Weights.z = (-frame + (framelength * 3)) / framelength;
		Weights.w = (frame - (framelength * 2)) / framelength;
	}
	if (frame < (framelength * 2))
	{
		Weights.x = 0;
		Weights.y = (-frame + (framelength * 2)) / framelength;
		Weights.z = (frame - (framelength)) / framelength;
		Weights.w = 0;
	}
	if (frame < (framelength))
	{
		Weights.x = (-frame + (framelength)) / framelength;
		Weights.y = frame / framelength;
		Weights.z = 0;
		Weights.w = 0;
	}
}

void Character::PlayAnimation(int animID)
{
	frame = 0;
	currentAnim = animID;
	renderObject = animations[animID].animation;
	framelength = animations[animID].framelength;
}

void Character::SetUpAnimation(RenderObject* anim, float framelengthin)
{
	Animation temp;
	temp.animation = anim;
	temp.framelength = framelengthin;
	animations.push_back(temp);
}

void Character::SetPrimaryAnimation(int animID)
{
	frame = 0;
	primaryAnim = animID;
	currentAnim = animID;
	renderObject = animations[animID].animation;
	framelength = animations[animID].framelength;
}

