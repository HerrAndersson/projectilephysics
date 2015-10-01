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

	skySphere = new GameObject(1, Assets->GetRenderObject(0), XMFLOAT3(512, 100, 512), XMFLOAT3(650, 650, 650), XMFLOAT3(0, 0, 0));
	uglyFix = new GameObject(1, Assets->GetRenderObject(4), XMFLOAT3(-150, 700, -150), XMFLOAT3(30, 30, 30), XMFLOAT3(90, 0, 0));

	PhysicsObject* sphere = new PhysicsObject(1, Assets->GetRenderObject(2), XMFLOAT3(256.0f, 50.0f, 256.0f), XMFLOAT3(60.0f, 60.0f, 60.0f), XMFLOAT3(0, 0, 0));
	sphere->WakePhysics();
	gameObjects.push_back(sphere);

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
	delete uglyFix;

	for (auto go : gameObjects) 
		delete go;
}

bool Game::Update(double frameTime, double gameTime)
{

	if (!Logic->Update(frameTime, gameTime, gameObjects, camera, skySphere, terrain))
		return false;

	return true;
}

bool Game::Render()
{
	bool result = true;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	worldMatrix = DirectX::XMMatrixIdentity();
	camera->GetProjectionMatrix(projectionMatrix);
	camera->GetViewMatrix(viewMatrix);

	//Renderer->ActivateShadowRendering(lightView, lightProj);
	//
	//for (int i = 0; i < (signed)gameObjects.size(); i++) 
	//	Renderer->RenderShadow(gameObjects.at(i));

	Renderer->BeginScene(0.05f, 0.05f, 0.05f, 1.0f);
	Renderer->SetDataPerFrame(viewMatrix, projectionMatrix, camera->GetPosition());

	Renderer->UseTerrainShader();
	Renderer->SetTerrainData(worldMatrix, XMFLOAT3(0, 0, 0), terrain);
	Renderer->RenderTerrain(terrain);

	Renderer->UseDefaultShader();
	
	for (int i = 0; i < (signed)gameObjects.size(); i++)
		Renderer->Render(gameObjects.at(i));

	Renderer->Render(uglyFix);
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
