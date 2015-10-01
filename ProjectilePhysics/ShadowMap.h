#pragma once
#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <d3dcompiler.h>
#include <string>

using namespace DirectX;

//Before rendering to shadow map:

//~ Activate FRONT-FACE culling
//~ Set vertex buffer before the Draw/DrawIndexed call
//~ Set topology

class ShadowMap
{
private:

	struct MatrixBufferPerObject
	{
		XMMATRIX modelWorld;
	};

	struct MatrixBufferPerFrame
	{
		XMMATRIX lightView;
		XMMATRIX lightProj;
	};

	int dimensions;

	ID3D11Texture2D*				shadowMap;
	ID3D11DepthStencilView*			shadowDepthView;
	ID3D11ShaderResourceView*		shadowResourceView;

	ID3D11DepthStencilState*        shadowDepthState;

	D3D11_VIEWPORT					shadowViewport;

	ID3D11InputLayout*              shadowInputLayout;
	ID3D11VertexShader*             shadowVS;

	ID3D11Buffer*					matrixBufferPerObject;
	ID3D11Buffer*					matrixBufferPerFrame;

public:

	ShadowMap(ID3D11Device* device, int dimensions, LPCWSTR vsFilename);
	virtual ~ShadowMap();

	ID3D11ShaderResourceView* GetShadowSRV();
	int GetSize();

	void ActivateShadowRendering(ID3D11DeviceContext* deviceContext);
	void SetDataPerObject(ID3D11DeviceContext* deviceContext, XMMATRIX& modelWorld);
	void SetDataPerFrame(ID3D11DeviceContext* deviceContext, XMMATRIX& lightView, XMMATRIX& lightProj);
};

