#pragma once
#include <Windows.h>
#include <DirectXMath.h>

#include "RenderModule.h"
#include "GameLogic.h"
#include "AssetManager.h"
#include "SaveLoadManager.h"

#include "AssetUtil.h"
#include "GameObject.h"
#include "PhysicsObject.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "Terrain.h"

using namespace DirectX;
using std::vector;

class Game
{
private:

	int screenWidth, screenHeight;
	const int shadowMapSize = 2048;

	Camera*			        camera;
	GameLogic*              Logic;
	RenderModule*           Renderer;
	AssetManager*			Assets;
	InputManager*			Input;

	vector<GameObject*>		gameObjects;
	Terrain*                terrain;
	GameObject*             skySphere;
	GameObject*			    sun;
	DirectionalLight*		sunLight;

public:

	Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen);
	~Game();

	bool Update(double frameTime, double gameTime);
	bool Render();

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);
};

