#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>
#include "D3DManager.h"
#include "AssetUtil.h"
#include "GameObject.h"
#include "lightObject.h"
#include "ShadowMap.h"
#include "GameLogic.h"

using DirectX::XMMATRIX;
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;

class RenderModule
{

private:
	
	
	struct LightBuffer
	{
		XMMATRIX lightView;
		XMMATRIX lightProj;

		//spotlight
		XMFLOAT3 lightPosSpot;
		float  lightRangeSpot;
		XMFLOAT3 lightDirSpot;
		float lightConeSpot;
		XMFLOAT3 lightAttSpot;
		float pad;
		XMFLOAT4 lightAmbientSpot;
		XMFLOAT4 lightDiffuseSpot;

		XMFLOAT3 lightPosPoint1;
		float pad2;
		XMFLOAT4 lightDiffusePoint1;

		XMFLOAT3 lightPosPoint2;
		float pad3;
		XMFLOAT4 lightDiffusePoint2;

		XMFLOAT3 lightPosPoint3;
		float pad4;
		XMFLOAT4 lightDiffusePoint3;

		int shadowMapSize;
	};

	struct MatrixBufferPerObject
	{
		XMMATRIX world;
		XMFLOAT3 colorModifier;
		float pad;
	};

	struct MatrixBufferPerWeightedObject
	{
		XMMATRIX world;
		XMFLOAT4X4 bones[30];
	};

	struct MatrixBufferPerBlendObject
	{
		XMMATRIX world;
		float weight[4];
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
	ID3D11InputLayout*		layoutPosUvNormIdxWei;
	ID3D11InputLayout*		layoutPosUvNorm3PosNorm;
	ID3D11Buffer*			matrixBufferPerObject;
	ID3D11Buffer*			matrixBufferPerWeightedObject;
	ID3D11Buffer*			matrixBufferPerBlendObject;
	ID3D11Buffer*			matrixBufferPerFrame;
	ID3D11Buffer*			lightBuffer;
	HWND					hwnd;

public:

	RenderModule(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen, int shadowMapSize);
	~RenderModule();

	bool InitializeSamplers();
	bool InitializeConstantBuffers();
	bool InitializeShader(WCHAR* vsFilename, WCHAR* psFilename);
	bool InitializeSkeletalShader(WCHAR* vsFilename, WCHAR* psFilename);
	bool InitializeBlendShader(WCHAR* vsFilename, WCHAR* psFilename);

	bool SetDataPerFrame(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, XMFLOAT3& camPos, LightObject* spotlight, LevelStates* levelstate);

	bool SetDataPerObject(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 colorModifier);
	bool SetDataPerBlendObject(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 colorModifier, XMFLOAT4& weights);
	bool SetDataPerSkeletalObject(XMMATRIX& worldMatrix, RenderObject* renderObject, bool isSelected, float frame);

	void UseDefaultShader();
	void ActivateShadowRendering(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix);
	void UseSkeletalShader();
	void UseBlendShader();

	void BeginScene(float red, float green, float blue, float alpha);

	bool Render(GameObject* gameObject);
	bool Render(GameObject* gameObject, float frame);
	bool Render(GameObject* gameObject, XMFLOAT4& weights);
	bool RenderShadow(GameObject* gameObject);

	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
};



