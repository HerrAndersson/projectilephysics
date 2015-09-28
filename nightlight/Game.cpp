#include "Game.h"

Game::Game(HINSTANCE hInstance, HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	camera = new CameraObject(XM_PI / 3, screenWidth, screenHeight, 0.1f, 1000.0f);

	float spotRange = 15.0f;
	float spotCone = 29.0f;
	float fov = (spotCone + 20.0f) * (XM_PI / 180);
	spotLight = new LightObject(fov, 1.0f, 0.1f, spotRange, spotCone, spotRange);

	spotLight->setAmbientColor(0.09f, 0.09f, 0.09f, 1.0f);
	spotLight->setDiffuseColor(0.55f, 0.45f, 0.2f, 1.0f);

	Renderer = new RenderModule(hwnd, screenWidth, screenHeight, fullscreen, shadowMapSize);
	Assets = new AssetManager(Renderer->GetDevice());
	saveLoadManager = SaveLoadManager();
	Input = new InputManager(hwnd, screenWidth, screenHeight);

	Logic = new GameLogic(Input);
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

bool Game::Update()
{
	bool result = true;
	
	result = Logic->Update();
	
	if (!result)
	{

	}

	return result;
}

bool Game::Render()
{
	bool result = true;

	//std::vector<GameObject*>* toRender = levelStates.currentLevel->GetGameObjects();

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	worldMatrix = DirectX::XMMatrixIdentity();
	camera->GetProjectionMatrix(projectionMatrix);
	camera->GetViewMatrix(viewMatrix);


	XMMATRIX lightView, lightProj;
	spotLight->getViewMatrix(lightView);
	spotLight->getProjMatrix(lightProj);

	Renderer->ActivateShadowRendering(lightView, lightProj);
	
	//for (int i = 0; i < (signed)toRender->size(); i++) 
	//	Renderer->RenderShadow(toRender->at(i));

	Renderer->BeginScene(0.05f, 0.05f, 0.05f, 1.0f);

	Renderer->SetDataPerFrame(viewMatrix, projectionMatrix, camera->GetPosition());
	Renderer->UseDefaultShader();
	
	//for (int i = 0; i < (signed)toRender->size(); i++)
	//	Renderer->Render(toRender->at(i), toRender->at(i)->GetWeights());

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
