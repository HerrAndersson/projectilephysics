#pragma once
#include "AssetUtil.h"
#include <DirectXMath.h>

#define TILE_SIZE 1.0f

using DirectX::XMMATRIX;
using namespace assetUtility;

struct Coord 
{
	int x;
	int y;

	Coord() 
	{
		x = -1;
		y = -1;
	}
	Coord(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	bool operator==(const Coord& other) 
	{
		if (x == other.x && y == other.y)
			return true;
		return false;
	}
	bool operator!=(const Coord& other) 
	{
		return !operator==(other);
	}
	Coord operator-(const Coord& other)
	{
		return Coord(x - other.x, y - other.y);
	}
};

class GameObject
{
protected:
	int id;
	Coord tileCoord;
	XMFLOAT3		position;
	XMFLOAT3		rotation = XMFLOAT3(0, 0, 0);
	XMVECTOR		forwardVector;
	RenderObject*	renderObject;
	bool			isSelected = false;
	XMFLOAT3		colorModifier = XMFLOAT3(0, 0, 0);
	XMFLOAT4 Weights = { 1, 0, 0, 0 };

public:
	enum GoTypes{ WALL, FLOOR, DOOR, STATICOBJECT, MOVABLEOBJECT, PRESSUREPLATE, CONTAINER, LEVER, MAINCHARACTER, SIDECHARACTER, SMALLENEMY, AVERAGEENEMY, LARGEENEMY, CORNER, CORNERIVERSE, BUTTON, PIPES, SHELF };

	XMFLOAT4 GetWeights(){ return Weights; }

	const XMFLOAT3 selectionColor = XMFLOAT3(0.21f, 0.21f, 0.17f);

	GameObject(int id, XMFLOAT3 position, float rotation, RenderObject* renderObject, int coordX, int coordY);
	virtual ~GameObject();

	XMMATRIX GetWorldMatrix();
	RenderObject* GetRenderObject();

	XMFLOAT3 GetColorModifier() const { return colorModifier; }
	void SetColorModifier(XMFLOAT3 val) { colorModifier = val; }

	bool IsSelected();
	void SetSelected(bool selected);

	int GetId() const { return id; }
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotationDeg();
	XMFLOAT3 GetRotationRad();
	Coord GetTileCoord() { return tileCoord; };
	XMVECTOR GetForwardVector();
	ID3D11ShaderResourceView* GetDiffuseTexture();
	ID3D11ShaderResourceView* GetSpecularTexture();

	void SetPosition(XMFLOAT3 pos);
	void SetTilePosition(Coord coord);
	void SetRotationDeg(XMFLOAT3 rot);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

};

