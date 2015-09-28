#pragma once
#include <Windows.h>
#include <DirectXMath.h>

#include "RenderModule.h"
#include "GameLogic.h"
#include "AiModule.h"
#include "AssetManager.h"
#include "LevelParser.h"
#include "SaveLoadManager.h"

#include "AssetUtil.h"
#include "GameObject.h"
#include "cameraObject.h"
#include "lightObject.h"
#include "Sounds.h"

using namespace DirectX;
using std::vector;

class Game
{
private:

	int screenWidth, screenHeight;
	const int shadowMapSize = 2048;

	std::string saveFilePath = "nightlight.sav";

	LevelStates             levelStates;

	vector<Enemy*>		    enemies;

	CameraObject*			camera;
	LightObject*			spotLight;
	Character*				character;
	Character*				grandpa;

	GameLogic*              Logic;
	RenderModule*           Renderer;
	AiModule*               AI;
	AssetManager*			Assets;
	LevelParser*			levelParser;
	SaveLoadManager			saveLoadManager;
	InputManager*			Input;
	Sounds*					sounds;

	const bool debugDisableWallRendering = false;
	const bool debugRenderEnemyPaths = false;
	const bool debugShowFps = false;

public:

	Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen);
	~Game();

	bool Update();
	bool Render();

	void UpdateCharacterAnimation();
	bool GetDebugShowFps() { return debugShowFps; }

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);
};

