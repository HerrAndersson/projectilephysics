#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	camera = new Camera(XM_PI / 3, screenWidth, screenHeight, 0.1f, 1000.0f);

	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen, shadowMapSize);
	Assets = new AssetManager(Renderer->GetDevice());
	Input = new InputManager(hwnd, screenWidth, screenHeight);

	Logic = new GameLogic(Input);

	GameObject* sphere = new GameObject(1, XMFLOAT3(0, 0, 0), 0.0f, Assets->GetRenderObject(3));

	gameObjects.push_back(sphere);
}

Game::~Game()
{
	delete Logic;
	delete Renderer;
	delete Assets;
	delete Input;
	delete camera;
}

bool Game::Update(double gameTime)
{
	bool result = true;

	XMFLOAT3 pos = gameObjects.at(0)->GetPosition();
	gameObjects.at(0)->SetPosition(XMFLOAT3(pos.x, pos.y + 0.2f, pos.z));

	result = Logic->Update();
	
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
	Renderer->UseDefaultShader();
	
	for (int i = 0; i < (signed)gameObjects.size(); i++)
		Renderer->RenderShadow(gameObjects.at(i));

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
