#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	camera = new Camera(XM_PI / 2.2f, screenWidth, screenHeight, 0.1f, 3000.0f);
	camera->SetPosition(XMFLOAT3(512, 50, 50));
	camera->SetLookAt(0, 0, 1);

	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen, shadowMapSize);
	Assets = new AssetManager(Renderer->GetDevice());
	Input = new InputManager(hwnd, screenWidth, screenHeight);

	Logic = new GameLogic(Input);

	//Objects
	skySphere = new GameObject(ObjectTypes::STATIC, Assets->GetRenderObject(0), XMFLOAT3(512, 100, 512), XMFLOAT3(850, 850, 850), XMFLOAT3(0, 0, 0));
	sun = new GameObject(ObjectTypes::STATIC, Assets->GetRenderObject(4), XMFLOAT3(512, 500, -400), XMFLOAT3(10, 10, 10), XMFLOAT3(0, 0, 0));

	sunCam = new Camera(XM_PI / 2.2f, 1, 1, 0.1f, 3000.0f);
	sunCam->SetPosition(sun->GetPosition());
	sunCam->SetRotation(XMFLOAT3(45.0f, 0.0f, 0.0f));

	GameObject* cannonBase = new GameObject(ObjectTypes::STATIC, Assets->GetRenderObject(5), XMFLOAT3(480, 10, 128), XMFLOAT3(MeterToUnits(1.5f), MeterToUnits(2.5f), MeterToUnits(1.8f)), XMFLOAT3(-20, 0, 0));
	gameObjects.push_back(cannonBase);

	cannon = new GameObject(ObjectTypes::CANNON, Assets->GetRenderObject(3), GameConstants::CANNONBALL_START_POS, XMFLOAT3(MeterToUnits(0.4f), MeterToUnits(0.4f), MeterToUnits(3.0f)), XMFLOAT3(-45, 0, 0));
	gameObjects.push_back(cannon);

	XMFLOAT3 pos = GameConstants::CANNONBALL_START_POS;
	for (int i = 0; i < 1000; i++)
	{
		if (i % 100 == 0)
		{
			GameObject* a = new GameObject(ObjectTypes::STATIC, Assets->GetRenderObject(5), XMFLOAT3(pos.x - 40, 3, pos.z + i), XMFLOAT3(1, 1, 1), XMFLOAT3(0, 0, 0));
			gameObjects.push_back(a);
			GameObject* b = new GameObject(ObjectTypes::STATIC, Assets->GetRenderObject(2), XMFLOAT3(pos.x - 40, 4, pos.z + i), XMFLOAT3(0.5, 0.5, 0.5), XMFLOAT3(0, 0, 0));
			gameObjects.push_back(b);
		}

		if (i < 10)
		{
			GameObject* a = new GameObject(ObjectTypes::STATIC, Assets->GetRenderObject(5), XMFLOAT3(pos.x - 41, 3, pos.z + i), XMFLOAT3(1, 1, 1), XMFLOAT3(0, 0, 0));
			gameObjects.push_back(a);
			GameObject* b = new GameObject(ObjectTypes::STATIC, Assets->GetRenderObject(2), XMFLOAT3(pos.x - 40, 3, pos.z + i), XMFLOAT3(0.5, 0.5, 0.5), XMFLOAT3(0, 0, 0));
			gameObjects.push_back(b);

			GameObject* c = new GameObject(ObjectTypes::STATIC, Assets->GetRenderObject(5), XMFLOAT3(pos.x - 41, (float)3 + i, pos.z), XMFLOAT3(1, 1, 1), XMFLOAT3(0, 0, 0));
			gameObjects.push_back(c);
			GameObject* d = new GameObject(ObjectTypes::STATIC, Assets->GetRenderObject(2), XMFLOAT3(pos.x - 40, (float)3 + i, pos.z), XMFLOAT3(0.5, 0.5, 0.5), XMFLOAT3(0, 0, 0));
			gameObjects.push_back(d);
		}
		
	}

	for (size_t i = 0; i < 10; i++)
	{
		PhysicsObject* sphere = new PhysicsObject(ObjectTypes::PHYSICS, Assets->GetRenderObject(2), GameConstants::CANNONBALL_START_POS, XMFLOAT3(MeterToUnits(0.225f), MeterToUnits(0.225f), MeterToUnits(0.225f)), XMFLOAT3(0, 0, 0), PhysicsConstants::IRON_DENSITY, UnitsToMeter(MeterToUnits(0.225f / 2)));
		projectiles.push_back(sphere);
	}

	terrain = new Terrain(Renderer->GetDevice(), "Assets/Textures/heightmap01f_small.bmp", 12.0f, 4.0f,
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
	delete sunCam;
	delete sun;

	for (auto go : gameObjects) 
		delete go;

	for (auto p : projectiles)
		delete p;
}

bool Game::Update(double frameTime, double gameTime)
{

	if (!Logic->Update(frameTime, gameTime, projectiles, camera, skySphere, terrain, cannon))
		return false;

	/*XMFLOAT3 pos = sun->GetPosition();
	pos.x = 512 + sin(float(gameTime / 10000.0f)) * 500;
	pos.z = 512 + cos(float(gameTime / 10000.0f)) * 500;

	sun->SetPosition(pos);
	sunLight->SetPosition(pos);*/

	return true;
}

bool Game::Render()
{
	bool result = true;

	XMMATRIX viewMatrix, projectionMatrix;
	XMMATRIX lightView, lightProjection;

	camera->GetProjectionMatrix(projectionMatrix);
	camera->GetViewMatrix(viewMatrix);

	sunCam->GetViewMatrix(lightView);
	sunCam->GetProjectionMatrix(lightProjection);

	//camera->GetProjectionMatrix(lightProjection);
	//camera->GetViewMatrix(lightView);

	//sunCam->GetViewMatrix(viewMatrix);
	//sunCam->GetProjectionMatrix(projectionMatrix);

	///////////////////////////////////////////////////////////// Shadows /////////////////////////////////////////////////////////////
	Renderer->ActivateShadowRendering(lightView, lightProjection);

	for (int i = 0; i < (signed)gameObjects.size(); i++) 
		Renderer->RenderShadow(gameObjects.at(i));

	for (int i = 0; i < (signed)projectiles.size(); i++)
		Renderer->RenderShadow(projectiles.at(i));

	Renderer->RenderShadow(cannon);

	//Render last of all shadow objects
	Renderer->SetCullingState(CullingState::BACK);
	Renderer->RenderShadow(terrain);

	//Shadows funkar inte!
	////////////////////////////////////////////////////////////// Normal //////////////////////////////////////////////////////////////
	Renderer->BeginScene(0.05f, 0.05f, 0.05f, 1.0f);
	Renderer->SetDataPerFrame(viewMatrix, projectionMatrix, camera->GetPosition(), sunCam->GetPosition(), lightView, lightProjection);

	Renderer->UseTerrainShader();
	Renderer->SetTerrainData(XMMatrixIdentity(), XMFLOAT3(0, 0, 0), terrain);
	Renderer->RenderTerrain(terrain);

	Renderer->UseDefaultShader();
	
	for (int i = 0; i < (signed)gameObjects.size(); i++)
		Renderer->Render(gameObjects.at(i));

	for (int i = 0; i < (signed)projectiles.size(); i++)
		Renderer->Render(projectiles.at(i));

	//Renderer->Render(sun);

	Renderer->SetCullingState(CullingState::FRONT);
	Renderer->Render(skySphere);

	RenderText();

	Renderer->EndScene();

	return result;
}

void Game::RenderText()
{
	float length = 0;
	bool found = false;
	for (unsigned int i = 0; i < projectiles.size() - 1 && !found; i++)
	{
		if (projectiles.at(i)->IsUsed() && !projectiles.at(i + 1)->IsUsed())
		{
			length = projectiles.at(i)->GetPosition().z - GameConstants::CANNONBALL_START_POS.z;
			found = true;
		}
	}

	if (!found)
		if (projectiles.at(projectiles.size() - 1)->IsUsed())
			length = projectiles.at(projectiles.size() - 1)->GetPosition().z - GameConstants::CANNONBALL_START_POS.z;

	int x1 = 0, x2 = 0;
	int x3 = 0, x4 = 0;
	int y1 = 0, y2 = 0;
	TruncateOne(float(360 - cannon->GetRotation().x), x1, x2);
	TruncateOne(Logic->GetLaunchSpeed(), y1, y2);
	TruncateOne(float(cannon->GetRotation().y), x3, x4);

	if (x3 > 20)
	{
		x3 = x3 - 360;
		if (x3 > -20)
			x4 = 9 - x4;
		else
			x4 = 0;
	}

	string s1;
	if (Logic->AirResistanceOn())
		s1 = "ON";
	else
		s1 = "OFF";

	string s2;
	if (Logic->ResponseForceOn())
		s2 = "ON";
	else
		s2 = "OFF";

	string s = "Angle Z: " + to_string(x1) + "." + to_string(x2)
		+ "\n" + "Angle Y: " + to_string(x3) + "." + to_string(x4)
		+ "\n" + "Air resistance: " + s1
		+ "\n" + "Response: " + s2
		+ "\n" + "Launch speed: " + to_string(y1) + "." + to_string(y2)
		+ "\n" + "Last length: " + to_string(UnitsToMeter(length));

	Renderer->DrawString(wstring(s.begin(), s.end()), (FLOAT)20.0f, (FLOAT)7, (FLOAT)5, 0xff000000);
}

void* Game::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void Game::operator delete(void* p)
{
	_mm_free(p);
}
