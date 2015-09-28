#pragma once
#include <Windows.h>
#include <iostream>
#include <math.h>
#include "InputManager.h"
#include "cameraObject.h"
#include "lightObject.h"
#include "Character.h"
#include "Level.h"
#include "AiModule.h"
#include "LevelParser.h"
#include "Sounds.h"

struct LevelStates
{
	int currentLevelNr = -1;
	int savedLevelNr = -1;
	Level* currentLevel = nullptr;
	Level* loadedLevel = nullptr;
	Level* menuLevel = nullptr;
	LevelParser* levelParser = nullptr;
};

class GameLogic
{
private:
	enum Axis { X, Y, BOTH };
	enum Direction { NONE, RIGHT, UP, LEFT, DOWN };

	InputManager*  Input;
	AiModule*      AI;

	bool quitGame = true;

	int screenWidth;
	int screenHeight;

	bool leftMouseLastState = false;
	bool moveObjectMode = false;
	bool loadedLevelMoveObjectMode = false;
	int loadedLevelMoveObjectModeAxis = Axis::BOTH;
	XMFLOAT3 loadedLevelCharacterRot;
	int moveObjectModeAxis = Axis::BOTH;
	XMFLOAT3 movableObjectTilePos;

	int resetLevelTimer = 0;
	bool restart = false;

	GameObject* selectedObject = nullptr;
	Button* selectedButton = nullptr;

	bool ManageLevelStates(LevelStates & levelStates, Character* character, vector<Enemy*>& enemies, LightObject* spotLight, Character* grandpa, Sounds* sounds);

	bool UpdatePlayer(LevelStates& levelStates, Character* player, CameraObject* camera, LightObject* spotLight, vector<Enemy*>& enemies, Sounds* sounds);
	bool UpdateSpotLight(LevelStates& levelStates, Character* player, CameraObject* camera, LightObject* spotlight, vector<Enemy*>& enemies);
	bool UpdateAI(vector<Enemy*>& enemies, Character* player, LightObject* spotlight);

public:
	enum SelectionTypes { BUTTON, LEVER, MOVABLEOBJECT };

	GameLogic(AiModule* AI, InputManager* Input);
	~GameLogic();

	bool Update(LevelStates& levelStates, Character* gameObject, CameraObject* camera, LightObject* spotLight, vector<Enemy*>& enemies, Character* grandpa, Sounds* sounds);

	void SelectObject(GameObject* newSelectedObject);
	void SelectButton(Button* newSelectedButton);

	GameObject* GetSelectedObject();
	bool GetMoveObjectMode();
	void UpdatePointLights(Level* currentLevel);
};

