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
		//Put this in here
		//int shadowMapSize;
	};

	D3DManager*				d3d;
	ShadowMap*				shadowMap;

	//Vertex shaders
	ID3D11VertexShader*		vertexShader;
	ID3D11VertexShader*		skeletalVertexShader;
	ID3D11VertexShader*		blendVertexShader;

	//Pixel shaders
	ID3D11PixelShader*		pixelShader;

	//Sampler states
	ID3D11SamplerState*		sampleStateWrap;
	ID3D11SamplerState*		sampleStateClamp;
	ID3D11SamplerState*     sampleStateComparison;

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
	bool InitializeShader(WCHAR* vsFilename, WCHAR* psFilename);

	bool SetDataPerFrame(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, XMFLOAT3& camPos);
	bool SetDataPerObject(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 colorModifier);

	void UseDefaultShader();
	void ActivateShadowRendering(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix);

	void BeginScene(float red, float green, float blue, float alpha);

	bool Render(GameObject* gameObject);
	bool RenderShadow(GameObject* gameObject);
	bool RenderTerrain(Terrain* terrain);

	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
};



