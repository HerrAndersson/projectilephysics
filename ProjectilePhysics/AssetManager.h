#pragma once
#include "AssetUtil.h"

using namespace std;
using namespace AssetUtility;

class AssetManager
{

private:

	ID3D11Device* device;

	vector<Model*> models;
	vector<ID3D11ShaderResourceView*> textures;
	vector<RenderObject*> renderObjects;

	void LoadModel(string file_path);

	void CreateRenderObject(int modelID, int diffuseID);

public:

	AssetManager(ID3D11Device* device);
	~AssetManager();

	RenderObject* GetRenderObject(int id);
	ID3D11ShaderResourceView* LoadTexture(string file_path);
};

