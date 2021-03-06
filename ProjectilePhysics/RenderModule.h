#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "D3DManager.h"
#include "AssetUtil.h"
#include "GameObject.h"
#include "DirectionalLight.h"
#include "ShadowMap.h"
#include "GameLogic.h"
#include "Terrain.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

class RenderModule
{

private:

	struct MatrixBufferPerObject
	{
		XMMATRIX world;
		XMFLOAT3 colorModifier;
		float pad;
	};

	struct MatrixBufferPerFrame
	{
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
		XMFLOAT3 camPos;
		float pad;
	};

	struct LightBuffer
	{
		XMMATRIX lightView;
		XMMATRIX lightProjection;
		XMFLOAT3 lightPos;
		int shadowMapSize;
	};

	D3DManager*				d3d;
	ShadowMap*				shadowMap;

	//Vertex shaders
	ID3D11VertexShader*		vsDefault;

	//Pixel shaders
	ID3D11PixelShader*		psDefault;
	ID3D11PixelShader*		psTerrain;

	//Sampler states
	ID3D11SamplerState*		sampleStateWrap;
	ID3D11SamplerState*		sampleStateClamp;
	ID3D11SamplerState*     sampleStateComparison;
	ID3D11SamplerState*     sampleStatePoint;

	//Other
	ID3D11InputLayout*		layoutPosUvNorm;
	ID3D11Buffer*			matrixBufferPerObject;
	ID3D11Buffer*			matrixBufferPerFrame;
	ID3D11Buffer*			lightBuffer;
	HWND					hwnd;

public:

	RenderModule(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen, int shadowMapSize);
	~RenderModule();

	bool InitializeSamplers();
	bool InitializeConstantBuffers();
	bool InitializeShaders(WCHAR* vsDefaultName, WCHAR* psDefaultName, WCHAR* psTerrainName);

	bool SetDataPerFrame(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, XMFLOAT3 camPos, XMFLOAT3 lightPos, XMMATRIX& lightView, XMMATRIX& lightProjection);
	bool SetDataPerObject(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 colorModifier);
	bool SetTerrainData(XMMATRIX& worldMatrix, XMFLOAT3 colorModifier, Terrain* terrain);

	void UseDefaultShader();
	void UseTerrainShader();
	void ActivateShadowRendering(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix);
	void SetCullingState(CullingState state);

	void BeginScene(float red, float green, float blue, float alpha);

	bool Render(GameObject* gameObject);
	bool RenderShadow(GameObject* gameObject);
	bool RenderShadow(Terrain* terrain);
	bool RenderTerrain(Terrain* terrain);

	void DrawString(wstring text, FLOAT fontSize, FLOAT posX, FLOAT posY, UINT32 color);

	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
};



