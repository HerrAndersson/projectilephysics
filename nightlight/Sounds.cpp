#include "Sounds.h"


Sounds::Sounds()
{
	YSE::System().init();

	menuMusic.create("Assets/Sounds/menuMusic.ogg");
	menuMusic.setLooping(true);
	menuMusic.setRelative(true);

	gameMusic.create("Assets/Sounds/gameMusic.ogg");
	//gameMusic.setLooping(true);

	walk.create("Assets/Sounds/walk.ogg");
	walk.setRelative(true);
	walk.setSpeed(1.3f);
	walk.setVolume(0.4f);
	
	hit.create("Assets/Sounds/hit.ogg");
	hit.setRelative(true);

	doorOpen.create("Assets/Sounds/doorOpen.ogg");
	doorOpen.setDoppler(false);
	doorOpen.setVolume(0.7f);

	leverFailed.create("Assets/Sounds/leverFailed.ogg");
	leverFailed.setRelative(true);
	leverFailed.setVolume(0.4f);

	dies.create("Assets/Sounds/dies.ogg");
	dies.setRelative(true);

	endDoorOpen.create("Assets/Sounds/endDoorOpen.ogg");

	moveBox.create("Assets/Sounds/moveBox.ogg");
	moveBox.setRelative(true);
	moveBox.setSpeed(2);
	moveBox.setVolume(0.5f);

	grabReleaseBox.create("Assets/Sounds/grabReleaseBox.ogg");
	grabReleaseBox.setRelative(true);
	//grabReleaseBox.setSpeed(0.5);
	grabReleaseBox.setVolume(0.5f);

	shadowContainerActivated.create("Assets/Sounds/shadowContainerActivated.ogg");
}


Sounds::~Sounds()
{
	YSE::System().close();
}

void Sounds::Update(Character* player)
{
	XMFLOAT3 playerPos = player->GetPosition();
	YSE::Listener().setPosition(YSE::Vec(playerPos.x, playerPos.y, playerPos.z));

	YSE::System().update();
}