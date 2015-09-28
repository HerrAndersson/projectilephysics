#pragma once
#include "AssetUtil.h"

using namespace std;
using namespace AssetUtility;

class AssetManager
{

public:

	AssetManager(ID3D11Device* device);
	~AssetManager();

	RenderObject* GetRenderObject(int id);

private:

	ID3D11Device* device;

	vector<Model*> models;
	vector<ID3D11ShaderResourceView*> textures;
	vector<RenderObject*> renderObjects;

	void LoadModel(string file_path);
	void LoadTexture(string file_path);

	void CreateRenderObject(int modelID, int diffuseID);
	ID3D11Buffer* CreateVertexBuffer(vector<Point>* purePoints, vector<XMFLOAT3>* normals, vector<XMFLOAT2>* UVs, vector<XMINT3>* vertexIndices);
};

