#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	camera = new Camera(XM_PI / 2.2f, screenWidth, screenHeight, 0.1f, 2000.0f);
	camera->SetPosition(XMFLOAT3(512, 50, 20));

	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen, shadowMapSize);
	Assets = new AssetManager(Renderer->GetDevice());
	Input = new InputManager(hwnd, screenWidth, screenHeight);

	Logic = new GameLogic(Input);


	//Objects
	skySphere = new GameObject(1, Assets->GetRenderObject(0), XMFLOAT3(512, 100, 512), XMFLOAT3(650, 650, 650), XMFLOAT3(0, 0, 0));
	sun = new GameObject(1, Assets->GetRenderObject(4), XMFLOAT3(-150, 700, -150), XMFLOAT3(30, 30, 30), XMFLOAT3(90, 0, 0));
	sunLight = new DirectionalLight(XM_PI / 2, 1.0f, 0.1, 2000.0f);
	sunLight->SetLookAt(XMFLOAT3(512, 0, 512));
	sunLight->SetPosition(sun->GetPosition());

	PhysicsObject* sphere = new PhysicsObject(1, Assets->GetRenderObject(1), XMFLOAT3(512.0f, 50.0f, 128.0f), XMFLOAT3(3.0f, 3.0f, 3.0f), XMFLOAT3(0, 0, 0));
	sphere->WakePhysics();
	gameObjects.push_back(sphere);

	for (size_t i = 0; i < 20; i++)
	{
		int x = rand() % 12 + 5;
		int y = rand() % 12 + 5;
		int z = rand() % 10 + 5;
		int m = rand() % 19 + 1;
		int n = rand() % 19 + 1;
		int o = rand() % 19 + 1;

		PhysicsObject* sphere = new PhysicsObject(1, Assets->GetRenderObject(2), XMFLOAT3(600 + x * m, y * n, 600 + z * o), XMFLOAT3(30.0f, 30.0f, 30.0f), XMFLOAT3(0, 0, 0));
		sphere->WakePhysics();
		gameObjects.push_back(sphere);
	}

	terrain = new Terrain(Renderer->GetDevice(), "Assets/Textures/heightmap01b.bmp", 5.0f,
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

	for (auto go : gameObjects) 
		delete go;
}

bool Game::Update(double frameTime, double gameTime)
{

	if (!Logic->Update(frameTime, gameTime, gameObjects, camera, skySphere, terrain))
		return false;

	XMFLOAT3 pos = sun->GetPosition();
	pos.x = 512 + sin(float(gameTime / 100000.0f)) * 1100;
	//pos.y = 50 + cos(float(gameTime / 1000.0f)) * 300;
	pos.z = 512 + cos(float(gameTime / 100000.0f)) * 1100;
	sun->SetPosition(pos);
	sunLight->SetPosition(pos);

	return true;
}

bool Game::Render()
{
	bool result = true;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightView, lightProjection;

	worldMatrix = DirectX::XMMatrixIdentity();
	camera->GetProjectionMatrix(projectionMatrix);
	camera->GetViewMatrix(viewMatrix);

	sunLight->GetViewMatrix(lightView);
	sunLight->GetProjectionMatrix(lightProjection);

	////Shadows
	//Renderer->ActivateShadowRendering(lightView, lightProjection);
	//
	//for (int i = 0; i < (signed)gameObjects.size(); i++) 
	//	Renderer->RenderShadow(gameObjects.at(i));

	//Renderer->RenderShadow(terrain);

	//Normal
	Renderer->BeginScene(0.05f, 0.05f, 0.05f, 1.0f);
	Renderer->SetDataPerFrame(viewMatrix, projectionMatrix, camera->GetPosition(), sunLight->GetPosition(), lightView, lightProjection);

	Renderer->UseTerrainShader();
	Renderer->SetTerrainData(worldMatrix, XMFLOAT3(0, 0, 0), terrain);
	Renderer->RenderTerrain(terrain);

	Renderer->UseDefaultShader();
	
	for (int i = 0; i < (signed)gameObjects.size(); i++)
		Renderer->Render(gameObjects.at(i));

	Renderer->Render(sun);
	Renderer->SetCullingState(2);
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
