#pragma once
#include "AssetUtil.h"
#include <DirectXMath.h>

using DirectX::XMMATRIX;
using namespace AssetUtility;

class GameObject
{

protected:

	int id;

	XMFLOAT3		position;
	XMFLOAT3		rotation = XMFLOAT3(0, 0, 0);
	XMFLOAT3		colorModifier = XMFLOAT3(0, 0, 0);
	XMVECTOR		forwardVector;

	RenderObject*	renderObject;


public:

	GameObject(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject);
	virtual ~GameObject();

	XMMATRIX GetWorldMatrix();
	RenderObject* GetRenderObject();

	XMFLOAT3 GetColorModifier()							const { return colorModifier; }
	void SetColorModifier(XMFLOAT3 val)						  { colorModifier = val; }

	int GetId()											const { return id; }
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotationDeg();
	XMFLOAT3 GetRotationRad();
	XMVECTOR GetForwardVector();
	ID3D11ShaderResourceView* GetDiffuseTexture();
	ID3D11ShaderResourceView* GetSpecularTexture();

	void SetPosition(XMFLOAT3 pos);
	void SetRotationDeg(XMFLOAT3 rot);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

};

