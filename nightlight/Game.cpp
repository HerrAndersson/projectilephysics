#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	camera = new Camera(XM_PI / 3, screenWidth, screenHeight, 0.1f, 1000.0f);

	float spotRange = 15.0f;
	float spotCone = 29.0f;
	float fov = (spotCone + 20.0f) * (XM_PI / 180);
	spotLight = new DirectionalLight(fov, 1.0f, 0.1f, spotRange, spotCone, spotRange);

	spotLight->setAmbientColor(0.09f, 0.09f, 0.09f, 1.0f);
	spotLight->setDiffuseColor(0.55f, 0.45f, 0.2f, 1.0f);

	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen, shadowMapSize);
	Assets = new AssetManager(Renderer->GetDevice());
	saveLoadManager = SaveLoadManager();
	Input = new InputManager(hwnd, screenWidth, screenHeight);

	Logic = new GameLogic(Input);

	GameObject* sphere = new GameObject(1, XMFLOAT3(0, 0, 0), 5.0f, Assets->GetRenderObject(3));

	gameObjects.push_back(sphere);
}

Game::~Game()
{
	delete Logic;
	delete Renderer;
	delete Assets;
	delete Input;

	delete camera;
	delete spotLight;
}

bool Game::Update(double gameTime)
{
	bool result = true;

	XMFLOAT3 pos = gameObjects.at(0)->GetPosition();
	gameObjects.at(0)->SetPosition(XMFLOAT3(pos.x, pos.y, pos.z+1));

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

	XMMATRIX lightView, lightProj;
	spotLight->getViewMatrix(lightView);
	spotLight->getProjMatrix(lightProj);

	Renderer->ActivateShadowRendering(lightView, lightProj);
	
	for (int i = 0; i < (signed)gameObjects.size(); i++) 
		Renderer->RenderShadow(gameObjects.at(i));

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
