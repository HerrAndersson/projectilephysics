#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	camera = new Camera(XM_PI / 2.2f, screenWidth, screenHeight, 0.1f, 1000.0f);
	camera->SetPosition(512, 20, 20);
	camera->SetLookAt(512, 10, 512);

	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen, shadowMapSize);
	Assets = new AssetManager(Renderer->GetDevice());
	Input = new InputManager(hwnd, screenWidth, screenHeight);

	Logic = new GameLogic(Input);

	PhysicsObject* sphere = new PhysicsObject(1, Assets->GetRenderObject(3), XMFLOAT3(256.0f, 50.0f, 256.0f), XMFLOAT3(200.0f, 50.0f, 50.0f), XMFLOAT3(0, 0, 45));
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
}

bool Game::Update(double gameTime)
{
	bool result = true;

	result = Logic->Update(gameTime, gameObjects, camera);
	
	if (!result)
	{

	}

	return result;
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
