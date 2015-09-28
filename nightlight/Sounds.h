#pragma once
#include "yse.hpp"
#include "Character.h"

class Sounds
{
public:
	YSE::sound menuMusic;
	YSE::sound gameMusic;
	YSE::sound walk;
	YSE::sound hit;
	YSE::sound doorOpen;
	YSE::sound leverFailed;
	YSE::sound dies;
	YSE::sound endDoorOpen;
	YSE::sound moveBox;
	YSE::sound grabReleaseBox;
	YSE::sound shadowContainerActivated;
	//YSE::sound shadowSound;

	Sounds();
	~Sounds();
	void Update(Character* player);
};

