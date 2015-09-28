#pragma once
#include <Windows.h>
#include <DirectXMath.h>

#include "RenderModule.h"
#include "GameLogic.h"
#include "AssetManager.h"
#include "SaveLoadManager.h"

#include "AssetUtil.h"
#include "GameObject.h"
#include "cameraObject.h"
#include "lightObject.h"

using namespace DirectX;
using std::vector;

class Game
{
private:

	int screenWidth, screenHeight;
	const int shadowMapSize = 2048;

	std::string saveFilePath = "nightlight.sav";

	CameraObject*			camera;
	LightObject*			spotLight;

	GameLogic*              Logic;
	RenderModule*           Renderer;
	AssetManager*			Assets;
	SaveLoadManager			saveLoadManager;
	InputManager*			Input;

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

