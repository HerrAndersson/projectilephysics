#include "GameLogic.h"
#include "Collision.h"

GameLogic::GameLogic(AiModule* AI, InputManager* Input)
{
	this->Input = Input;
	this->AI = AI;

}

GameLogic::~GameLogic()
{

}

bool GameLogic::Update(LevelStates& levelStates, Character* character, CameraObject* camera, LightObject* spotLight, vector<Enemy*>& enemies, Character* grandpa, Sounds* sounds)
{
	if (character->GetHitPoints() > 0)
	{
		if (!UpdatePlayer(levelStates, character, camera, spotLight, enemies, sounds))
			return false;
	}

	if (levelStates.currentLevel != levelStates.menuLevel)
	{
		if (!UpdateAI(enemies, character, spotLight))
			return false;
	}

	if (!ManageLevelStates(levelStates, character, enemies, spotLight, grandpa, sounds))
		return false;

	Input->Update();

	if (resetLevelTimer > 0)
	{
		resetLevelTimer--;
		if (resetLevelTimer <= 0)
			restart = true;
	}

	return true;
}

bool GameLogic::UpdateAI(vector<Enemy*>& enemies, Character* player, LightObject* spotlight)
{
	for (int i = 0; i < (signed)enemies.size();)
	{
		bool isAlive = AI->HandleAI(enemies.at(i), player, spotlight);
		if (isAlive)
			i++;
		else
			enemies.erase(enemies.begin() + i);
	}

	return true;
}

bool GameLogic::UpdatePlayer(LevelStates& levelStates, Character* character, CameraObject* camera, LightObject* spotlight, vector<Enemy*>& enemies, Sounds* sounds)
{
	Level* currentLevel = levelStates.currentLevel;
	XMFLOAT2 oldP = Input->GetMousePos();
	Input->HandleMouse();
	XMFLOAT2 newP = Input->GetMousePos();

	XMFLOAT3 pos = character->GetPosition();
	XMFLOAT3 rot = character->GetRotationDeg();

	int playerMoved = Direction::NONE;

	if (Input->KeyDown('w'))
	{
		if (moveObjectModeAxis == Axis::BOTH || moveObjectModeAxis == Axis::Y) 
		{
			pos = XMFLOAT3(pos.x, pos.y, pos.z + character->GetSpeed());
			playerMoved = Direction::UP;
		}
	}
	if (Input->KeyDown('s'))
	{
		if (moveObjectModeAxis == Axis::BOTH || moveObjectModeAxis == Axis::Y) 
		{
			pos = XMFLOAT3(pos.x, pos.y, pos.z - character->GetSpeed());
			playerMoved = Direction::DOWN;
		}
	}
	if (Input->KeyDown('a'))
	{
		if (moveObjectModeAxis == Axis::BOTH || moveObjectModeAxis == Axis::X) 
		{
			pos = XMFLOAT3(pos.x + character->GetSpeed(), pos.y, pos.z);
			playerMoved = Direction::LEFT;
		}
	}
	if (Input->KeyDown('d'))
	{
		if (moveObjectModeAxis == Axis::BOTH || moveObjectModeAxis == Axis::X) 
		{
			pos = XMFLOAT3(pos.x - character->GetSpeed(), pos.y, pos.z);
			playerMoved = Direction::RIGHT;
		}
	}

	if (Input->KeyDown('z'))
	{
		XMFLOAT4 c = spotlight->getAmbientColor();
		spotlight->setAmbientColor(c.x - 0.005f, c.y - 0.005f, c.z - 0.005f, 1.0f);
	}
	else if (Input->KeyDown('x'))
	{
		XMFLOAT4 c = spotlight->getAmbientColor();
		spotlight->setAmbientColor(c.x + 0.005f, c.y + 0.005f, c.z + 0.005f, 1.0f);
	}

	if (playerMoved)
	{
		sounds->walk.play();

		Coord movableObjectOffset;
		Coord nextMovableTileCoord = Coord();

		if (moveObjectMode) {
			Coord selectedObjectCurrentCoord = selectedObject->GetTileCoord();
			Coord characterCurrentCoord = character->GetTileCoord();

			int offsetX = selectedObjectCurrentCoord.x - characterCurrentCoord.x;
			int offsetY = selectedObjectCurrentCoord.y - characterCurrentCoord.y;

			if (offsetX < 0)
				offsetX = -1;
			else if (offsetX > 0)
				offsetX = 1;

			if (offsetY < 0)
				offsetY = -1;
			else if (offsetY > 0)
				offsetY = 1;

			movableObjectOffset = Coord(offsetX, offsetY);

			switch (playerMoved) 
			{
			case Direction::UP:
				nextMovableTileCoord = Coord(characterCurrentCoord.x, characterCurrentCoord.y - 2);
				break;
			case Direction::DOWN:
				nextMovableTileCoord = Coord(characterCurrentCoord.x, characterCurrentCoord.y + 2);
				break;
			case Direction::RIGHT:
				nextMovableTileCoord = Coord(characterCurrentCoord.x + 2, characterCurrentCoord.y);
				break;
			case Direction::LEFT:
				nextMovableTileCoord = Coord(characterCurrentCoord.x - 2, characterCurrentCoord.y);
				break;
			default:
				break;
			}
			bool result;

			XMFLOAT3 nextMovablePos = pos;
			nextMovablePos.x -= movableObjectOffset.x;
			nextMovablePos.z -= movableObjectOffset.y;

			Tile* nextMovableTile = currentLevel->getTile(nextMovableTileCoord);

			if (!nextMovableTile->IsWalkable(moveObjectMode, selectedObject) || 
				(nextMovableTile->getDoor() && !nextMovableTile->getDoor()->getIsOpen()) || 
				nextMovableTile->getLever())
			{
				
				XMFLOAT3 currentMovablePos = nextMovablePos;
				nextMovablePos = NextPositionFromCollision(result, currentMovablePos, 0.5f, nextMovableTileCoord);

				if (currentMovablePos.x != nextMovablePos.x || currentMovablePos.z != nextMovablePos.z)
				{
					pos.x += nextMovablePos.x - currentMovablePos.x;
					pos.z += nextMovablePos.z - currentMovablePos.z;
				}
			}

			selectedObject->SetPosition(nextMovablePos);
		}
		
		pos = ManageCollisions(this, currentLevel, character, pos, CollisionTypes::CHARACTER);
		
		if (moveObjectMode)
		{
			XMFLOAT3 movablePos = pos;
			movablePos.x -= movableObjectOffset.x;
			movablePos.z -= movableObjectOffset.y;

			float zOffset = abs(movablePos.z - movableObjectTilePos.z);
			float xOffset = abs(movablePos.x - movableObjectTilePos.x);

			
			if (zOffset >= 1 || xOffset >= 1) 
			{
				currentLevel->getTile((int)-movableObjectTilePos.x, (int)-movableObjectTilePos.z)->setMovableObject(nullptr);
				currentLevel->getTile(selectedObject->GetTileCoord())->setMovableObject((MovableObject*)selectedObject);
				movableObjectTilePos = movablePos;
				character->PlayAnimation(2);
			}
			if (!sounds->moveBox.isPlaying()){
				sounds->moveBox.play();
			}
		}
	}
	else 
	{
		sounds->walk.stop();
		if (sounds->moveBox.isPlaying()){
			sounds->moveBox.stop();
		}
	}
		

	if (Input->LeftMouseClicked())
	{
		Lever* lever = nullptr;
		MovableObject* movable = nullptr;

		if (selectedObject != nullptr)
		{
			int id = selectedObject->GetId();

			if (id == GameObject::GoTypes::MOVABLEOBJECT) 
			{
				movable = (MovableObject*)selectedObject;
				moveObjectMode = !moveObjectMode;
				character->PlayAnimation(2);
				sounds->grabReleaseBox.play();
			}
			if (id == GameObject::GoTypes::LEVER) 
			{
				character->PlayAnimation(1);
				lever = (Lever*)selectedObject;
				if (lever->getIsActivated()) 
				{
					lever->DeactivateLever();
				}
				else {
					lever->ActivateLever();
				}
			}
			else
			{
				character->PlayAnimation(0);
			}
		}

		if (moveObjectMode)
		{
			movableObjectTilePos = selectedObject->GetPosition();
			Coord movableCoord = selectedObject->GetTileCoord();
			Coord characterCoord = character->GetTileCoord();
			XMFLOAT3 characterRot = character->GetRotationDeg();

			pos = XMFLOAT3(-characterCoord.x - TILE_SIZE / 2, 0.0f, -characterCoord.y - TILE_SIZE / 2);

			if (movableCoord.y > characterCoord.y) 
			{
				characterRot.y = -90.0f;
				moveObjectModeAxis = Axis::Y;
			} else if (movableCoord.y < characterCoord.y) 
			{
				characterRot.y = 90.0f;
				moveObjectModeAxis = Axis::Y;
			} else if (movableCoord.x > characterCoord.x) 
			{
				characterRot.y = 0.0f;
				moveObjectModeAxis = Axis::X;
			} else if (movableCoord.x < characterCoord.x) 
			{
				characterRot.y = 180.0f;
				moveObjectModeAxis = Axis::X;
			}
			character->SetRotationDeg(characterRot);
		}
		else if (movable) 
		{
			Coord lastTileCoord = Coord((int)-movableObjectTilePos.x, (int)-movableObjectTilePos.z);
			Coord currentTileCoord = movable->GetTileCoord();
			Tile* lastTile = currentLevel->getTile(lastTileCoord);
			Tile* currentTile = currentLevel->getTile(currentTileCoord);
			
			if (lastTile->getMovableObject()) 
			{
				lastTile->setMovableObject(nullptr);
				currentTile->setMovableObject(movable);
			}

			movable->SetTilePosition(currentTileCoord);
			pos = ManageCollisions(this, currentLevel, character, pos, CollisionTypes::CHARACTER);
			moveObjectModeAxis = Axis::BOTH;
		}
	}

	if (!moveObjectMode)
	{
		//Only update if mouse or player moved
		if (oldP.x != newP.x || oldP.y != newP.y || playerMoved)
		{
			XMMATRIX v, p, vp;
			camera->GetViewMatrix(v);
			camera->GetProjectionMatrix(p);
			vp = v * p;

			XMVECTOR localSpace = XMVector3Transform(XMLoadFloat3(&pos), vp);
			float screenX = XMVectorGetX(localSpace) / XMVectorGetZ(localSpace);
			float screenY = XMVectorGetY(localSpace) / XMVectorGetZ(localSpace);

			XMFLOAT2 msp = Input->GetMousePosScreenSpace();
			double dx = (msp.x - screenX);
			double dy = (msp.y - screenY) / camera->GetAspectRatio();
			double angle = atan2(dy, dx) * (180 / XM_PI);

			rot = XMFLOAT3(0.0f, (float)angle, 0.0f);

			character->SetRotationDeg(rot);
		}
	}
	for (Enemy* e : enemies)
	{
		if (sqrt((pow((e->GetPosition().x - pos.x), 2)) + (pow((e->GetPosition().z - pos.z), 2))) < 0.7f && character->GetInvulTimer() <= 0)
		{
			character->PlayAnimation(3);
			character->SetHitPoints(character->GetHitPoints() - 1);
			if (character->GetHitPoints() <= 0){
				sounds->dies.play();
				character->GetRenderObject()->diffuseTexture = e->GetRenderObject()->diffuseTexture;//dead//todo
				character->SetPrimaryAnimation(5); 
				character->PlayAnimation(4);
				spotlight->setAmbientColor(0.1f, 0.07f, 0.07f, 1.0f);
				spotlight->setDiffuseColor(0, 0, 0, 0);
				resetLevelTimer = 180;
			}
			else
			{
				sounds->hit.play();
			}
			character->SetInvulTimer(180);
		}
	}
	if (character->GetHitPoints() > 0)
	{
		int invulTimer = character->GetInvulTimer();
		int blinkspeed = 4;
		if (character->GetHitPoints() > 0)//temporär ifsats för att förhindra krashar då hp kan gå under 1
			blinkspeed = character->GetHitPoints() * 10;
		if (invulTimer > 0)
		{
			int mod = invulTimer % blinkspeed;
			if (mod > ((blinkspeed / 2) - 1))
				character->SetSelected(true);
			if (mod < (blinkspeed / 2))
				character->SetSelected(false);

			character->SetInvulTimer(invulTimer - 1);
		}
	}

	camera->SetPosition(character->GetPosition().x, -12, character->GetPosition().z - 3.5f);
	camera->SetLookAt(character->GetPosition().x, 5.0f, character->GetPosition().z*1.005f);
	character->SetPosition(pos);
	character->SetMoved(playerMoved);
	currentLevel->setPlayerPosition(pos);

	UpdateSpotLight(levelStates, character, camera, spotlight, enemies);
	if (levelStates.currentLevel != levelStates.menuLevel)
		UpdatePointLights(levelStates.currentLevel);

	return true;
}

bool GameLogic::UpdateSpotLight(LevelStates& levelStates, Character* player, CameraObject* camera, LightObject* spotlight, vector<Enemy*>& enemies)
{
	XMFLOAT3 pForward;
	XMStoreFloat3(&pForward, player->GetForwardVector());
	spotlight->setDirection(pForward.x, pForward.y, pForward.z);

	float length = sqrt((pForward.x*pForward.x) + (pForward.y*pForward.y) + (pForward.z*pForward.z));

	pForward.x = pForward.x / length;
	pForward.y = pForward.y / length;
	pForward.z = pForward.z / length;

	XMFLOAT3 pPos = player->GetPosition();
	//offset light
	pPos.x -= pForward.x / 2.0f;
	pPos.z -= pForward.z / 2.0f;
	pPos.y -= 0.7f;
	spotlight->setPosition(pPos.x, pPos.y, pPos.z);

	XMFLOAT3 lookAt;
	lookAt.x = pPos.x + pForward.x;
	lookAt.y = pPos.y + pForward.y;
	lookAt.z = pPos.z + pForward.z;

	spotlight->setLookAt(lookAt);

	spotlight->generateViewMatrix();

	return true;
}

GameObject* GameLogic::GetSelectedObject()
{
	return selectedObject;
}
bool GameLogic::GetMoveObjectMode()
{
	return moveObjectMode;
}

void GameLogic::SelectObject(GameObject* newSelectedObject)
{
	if (newSelectedObject != nullptr)
	{
		SelectObject(nullptr);

		selectedObject = newSelectedObject;
		selectedObject->SetSelected((true));
	}
	else
	{
		if (selectedObject != nullptr)
		{
			selectedObject->SetSelected(false);
			selectedObject = nullptr;
		}
	}
}

void GameLogic::SelectButton(Button* newSelectedButton)
{
	if (newSelectedButton != nullptr)
	{
		SelectButton(nullptr);

		selectedButton = newSelectedButton;
		selectedButton->SetSelected((true));
	}
	else
	{
		if (selectedButton != nullptr)
		{
			selectedButton->SetSelected(false);
			selectedButton = nullptr;
		}
	}
}

bool GameLogic::ManageLevelStates(LevelStates &levelStates, Character* character, vector<Enemy*>& enemies, LightObject* spotLight, Character* grandpa, Sounds* sounds)
{
	Level* currentLevel = levelStates.currentLevel;
	Level* menuLevel = levelStates.menuLevel;
	Level* loadedLevel = levelStates.loadedLevel;

	Coord characterTileCoord = character->GetTileCoord();
	Tile* currentTile = currentLevel->getTile(characterTileCoord.x, characterTileCoord.y);
	Button* button = currentTile->getButton();

	if (button)
		SelectButton(button);
	else
		SelectButton(nullptr);

	//Choose the correct level
	if (Input->LeftMouseClicked())
	{
		if (currentLevel == menuLevel)
		{
			if (button != nullptr)
			{
				int buttonType = button->getButtonType();
				switch (buttonType)
				{
				case Button::ButtonTypes::START:
					levelStates.currentLevelNr = 1;
					SelectObject(nullptr);
					restart = true;
					break;
				case Button::ButtonTypes::CONTINUE:
					if (loadedLevel)
						levelStates.currentLevelNr = loadedLevel->GetLevelNr();
					else if (levelStates.savedLevelNr != -1)
						levelStates.currentLevelNr = levelStates.savedLevelNr;
					SelectObject(nullptr);
					break;
				case Button::ButtonTypes::EXIT:
					return false;
					break;
				default:
					break;
				}
			}
		}
	}
	else if (Input->EscClicked() && resetLevelTimer <= 0)
	{
		if (currentLevel->GetLevelNr() != menuLevel->GetLevelNr())
			levelStates.currentLevelNr = menuLevel->GetLevelNr();
		else
			if (loadedLevel)
				levelStates.currentLevelNr = loadedLevel->GetLevelNr();
	}

	//Check for end door. If found, go to next level
	Door* door = currentTile->getDoor();
	if (door && door->getDoorType() == door->DoorTypes::END_DOOR && door->getIsOpen())
	{
		levelStates.currentLevelNr++;
		character->SetHitPoints(3);
	}

	//Set the correct level
	if (currentLevel->GetLevelNr() != levelStates.currentLevelNr || restart)
	{
		if (levelStates.currentLevelNr == menuLevel->GetLevelNr())
		{
			loadedLevelCharacterRot = character->GetRotationDeg();
			loadedLevelMoveObjectMode = moveObjectMode;
			moveObjectMode = false;
			loadedLevelMoveObjectModeAxis = moveObjectModeAxis;
			moveObjectModeAxis = Axis::BOTH;

			currentLevel = menuLevel;
			character->SetTilePosition(currentLevel->getStartDoorCoord());
		}
		else if (!loadedLevel || loadedLevel->GetLevelNr() != levelStates.currentLevelNr || restart)
		{
			restart = false;
			character->SetPrimaryAnimation(0);
			character->SetHitPoints(3);
			spotLight->setAmbientColor(0.09f, 0.09f, 0.09f, 1.0f);
			spotLight->setDiffuseColor(0.55f, 0.45f, 0.2f, 1.0f);

			SelectObject(nullptr);
			moveObjectMode = false;
			moveObjectModeAxis = Axis::BOTH;

			if (loadedLevel)
			{
				delete loadedLevel;
				enemies.clear();
			}
			int debug = levelStates.currentLevelNr;
			debug = levelStates.levelParser->GetLevelCount();
			if (levelStates.currentLevelNr > levelStates.levelParser->GetLevelCount() - 1) 
			{
				loadedLevel = levelStates.levelParser->LoadLevel(1, enemies, *character, *grandpa, sounds);
				levelStates.currentLevelNr = menuLevel->GetLevelNr();
			}
			else
			{
				loadedLevel = levelStates.levelParser->LoadLevel(levelStates.currentLevelNr, enemies, *character, *grandpa, sounds);
				currentLevel = loadedLevel;
				character->SetTilePosition(currentLevel->getStartDoorCoord());
			}
		}
		else
		{		
			moveObjectMode = loadedLevelMoveObjectMode;
			moveObjectModeAxis = loadedLevelMoveObjectModeAxis;
			currentLevel = loadedLevel;
			character->SetRotationDeg(loadedLevelCharacterRot);
			character->SetPosition(currentLevel->getPlayerPostion());
		}

		if (levelStates.levelParser->isEnd == true)
		{
			grandpa->SetPosition(XMFLOAT3(-8.5f, 0.0f, -2.5f));
			grandpa->SetRotationDeg(XMFLOAT3(0,-90,0));
			grandpa->SetPrimaryAnimation(0);
			grandpa->PlayAnimation(2);
		}

		AI->ChangeLevel(currentLevel);
	}

	levelStates.currentLevel = currentLevel;
	levelStates.loadedLevel = loadedLevel;

	return true;
}

void GameLogic::UpdatePointLights(Level* currentLevel) 
{
	Door* endDoor = currentLevel->getTile(currentLevel->getEndDoor())->getDoor();
	LightObject* endDoorLight = currentLevel->GetLightAt(0);

	if (endDoor->getIsOpen())
		endDoorLight->setDiffuseColor(0.1f, 0.95f, 0.2f, 1.0f);
	else
		endDoorLight->setDiffuseColor(0.95f, 0.1f, 0.2f, 1.0f);
}
