#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	camera = new Camera(XM_PI / 2.2f, screenWidth, screenHeight, 0.1f, 3000.0f);
	camera->SetPosition(XMFLOAT3(512, 50, 50));

	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen, shadowMapSize);
	Assets = new AssetManager(Renderer->GetDevice());
	Input = new InputManager(hwnd, screenWidth, screenHeight);

	Logic = new GameLogic(Input);


	//Objects
	skySphere = new GameObject(ObjectTypes::STATIC, Assets->GetRenderObject(0), XMFLOAT3(512, 100, 512), XMFLOAT3(850, 850, 850), XMFLOAT3(0, 0, 0));
	sun = new GameObject(ObjectTypes::STATIC, Assets->GetRenderObject(4), XMFLOAT3(-150, 700, -150), XMFLOAT3(30, 30, 30), XMFLOAT3(90, 0, 0));
	sunLight = new DirectionalLight(XM_PI / 2, 1.0f, 0.1f, 3000.0f);
	sunLight->SetLookAt(XMFLOAT3(512, 0, 512));
	sunLight->SetPosition(sun->GetPosition());

	GameObject* cannonBase = new GameObject(ObjectTypes::STATIC, Assets->GetRenderObject(5), XMFLOAT3(480, 20, 128), XMFLOAT3(35, 35, 35), XMFLOAT3(0, 0, 0));
	gameObjects.push_back(cannonBase);

	cannon = new GameObject(ObjectTypes::CANNON, Assets->GetRenderObject(3), XMFLOAT3(480, 20, 142), XMFLOAT3(10, 10, 60), XMFLOAT3(-45, 0, 0));

	//for (size_t i = 0; i < 2000; i++)
	//{
	//	float x = float(rand() % 1023 + 1);
	//	float y = float(rand() % 50 + 5);
	//	float z = float(rand() % 6 + 1);


	//	PhysicsObject* sphere = new PhysicsObject(ObjectTypes::PHYSICS, Assets->GetRenderObject(2), XMFLOAT3(x, 10 + y, z * (i / 10)), XMFLOAT3(25.0f, 25.0f, 25.0f), XMFLOAT3(0, 0, 0), 10);
	//	gameObjects.push_back(sphere);
	//}

	for (size_t i = 0; i < 1; i++)
	{
		/*PhysicsObject* sphere = new PhysicsObject(ObjectTypes::PHYSICS, Assets->GetRenderObject(2), GameConstants::CANNONBALL_START_POS, XMFLOAT3(5.0f, 5.0f, 5.0f), XMFLOAT3(0, 0, 0), 10);*/
		PhysicsObject* sphere = new PhysicsObject(ObjectTypes::PHYSICS, Assets->GetRenderObject(2), XMFLOAT3(480, 20, 180), XMFLOAT3(5.0f, 5.0f, 5.0f), XMFLOAT3(0, 0, 0), 10);
		gameObjects.push_back(sphere);
	}

	terrain = new Terrain(Renderer->GetDevice(), "Assets/Textures/heightmap01f.bmp", 3.0f,
		Assets->LoadTexture("Assets/Textures/blendmap.png"),
		Assets->LoadTexture("Assets/Textures/grass1.png"),
		Assets->LoadTexture("Assets/Textures/stone1.png"),
		Assets->LoadTexture("Assets/Textures/sand.png"));

}

Game::~Game()
{
	delete Logic;
	delete Renderer;
	delete Assets;
	delete Input;
	delete camera;
	delete terrain;
	delete skySphere;
	delete sun;
	delete sunLight;
	delete cannon;

	for (auto go : gameObjects) 
		delete go;
}

bool Game::Update(double frameTime, double gameTime)
{

	if (!Logic->Update(frameTime, gameTime, gameObjects, camera, skySphere, terrain, cannon))
		return false;

	XMFLOAT3 pos = sun->GetPosition();

	pos.x = 512 + sin(float(gameTime / 10000.0f)) * 500;
	pos.z = 512 + cos(float(gameTime / 10000.0f)) * 500;

	sun->SetPosition(pos);
	sunLight->SetPosition(pos);

	return true;
}

bool Game::Render()
{
	bool result = true;

	XMMATRIX viewMatrix, projectionMatrix;
	XMMATRIX lightView, lightProjection;

	camera->GetProjectionMatrix(projectionMatrix);
	camera->GetViewMatrix(viewMatrix);

	sunLight->GetViewMatrix(lightView);
	sunLight->GetProjectionMatrix(lightProjection);

	///////////////////////////////////////////////////////////// Shadows /////////////////////////////////////////////////////////////
	Renderer->ActivateShadowRendering(lightView, lightProjection);
	
	for (int i = 0; i < (signed)gameObjects.size(); i++) 
		Renderer->RenderShadow(gameObjects.at(i));

	Renderer->RenderShadow(cannon);

	//Render last of all shadow objects
	Renderer->SetCullingState(CullingState::BACK);
	Renderer->RenderShadow(terrain);

	//Shadows funkar inte! Fel SRV som samplas ifr�n?
	////////////////////////////////////////////////////////////// Normal //////////////////////////////////////////////////////////////
	Renderer->BeginScene(0.05f, 0.05f, 0.05f, 1.0f);
	Renderer->SetDataPerFrame(viewMatrix, projectionMatrix, camera->GetPosition(), sunLight->GetPosition(), lightView, lightProjection);

	Renderer->UseTerrainShader();
	Renderer->SetTerrainData(XMMatrixIdentity(), XMFLOAT3(0, 0, 0), terrain);
	Renderer->RenderTerrain(terrain);

	Renderer->UseDefaultShader();
	
	for (int i = 0; i < (signed)gameObjects.size(); i++)
		Renderer->Render(gameObjects.at(i));

	Renderer->Render(sun);
	Renderer->Render(cannon);

	Renderer->SetCullingState(CullingState::FRONT);
	Renderer->Render(skySphere);

	Renderer->EndScene();

	return result;
}

void* Game::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void Game::operator delete(void* p)
{
	_mm_free(p);
}
