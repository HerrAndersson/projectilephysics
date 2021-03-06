#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <stdexcept>
#include <windows.h>
#include "AssetUtil.h"
#include "AssetManager.h"

using namespace std;
using namespace AssetUtility;

class Terrain
{
private:

	//For texture-blending to work this has to be set to 1. 
	//If no blending is used then repeating texture works
	const int TEXTURE_REPEAT = 1;

	struct float3
	{
		float x, y, z;
	};

	struct HeightMap
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	int terrainWidth;
	int terrainHeight;
	int vertexCount;
	int indexCount;

	float sizeFactor;

	HeightMap* heightMap;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* textureArray[4];

	void InitializeBuffers(ID3D11Device* device);

	bool LoadHeightMap(char* filename);
	void NormalizeHeightMap(float factor);
	void CalculateNormals();
	void CalculateTextureCoordinates();
	void EnlargeTerrain(float sizeFactor);

	float GetHeightAt(int x, int z);

public:

	Terrain(ID3D11Device* device, char* heightMapName, float normalizeFactor, float sizeFactor, ID3D11ShaderResourceView* blendMap, ID3D11ShaderResourceView* grass, ID3D11ShaderResourceView* stone, ID3D11ShaderResourceView* sand);
	~Terrain();

	float GetY(float x, float z);
	XMFLOAT3 GetNormalAt(float x, float z);

	int GetVertexCount();

	void GetBuffers(ID3D11Buffer*& vertexBuffer, ID3D11Buffer*& indexBuffer);
	ID3D11ShaderResourceView** GetTextures();
};
