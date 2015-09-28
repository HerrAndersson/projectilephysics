#include "AssetManager.h"

AssetManager::AssetManager(ID3D11Device* device)
{
	this->device = device;

	//vector<string> modelNames;
	//FileToStrings("Assets/models.txt", modelNames);
	//for (int i = 0; i < (signed)modelNames.size(); i++)
	//	LoadModel("Assets/Models/" + modelNames[i]);

	//vector<string> textureNames;
	//FileToStrings("Assets/textures.txt", textureNames);
	//for (int i = 0; i < (signed)textureNames.size(); i++)
	//	LoadTexture("Assets/Textures/" + textureNames[i]);

	//vector<string> renderObjectIDs;
	//FileToStrings("Assets/renderObjects.txt", renderObjectIDs);
	//for (int i = 0; i < (signed)renderObjectIDs.size(); i++)
	//{
	//	vector<int> IDs = StringToIntArray(renderObjectIDs[i]);
	//	CreateRenderObject(IDs[0], IDs[1], IDs[2]);
	//}
};

AssetManager::~AssetManager()
{
	for (auto m : models) delete m;
	models.clear();

	for (auto t : textures) t->Release();
	textures.clear();

	for (auto ro : renderObjects) delete ro;
	renderObjects.clear();
};

void AssetManager::LoadModel(string file_path)
{
	Model* model = new Model();

	ifstream infile;
	infile.open(file_path.c_str(), ifstream::binary);

	if (!infile.is_open())
	{
		string outputstring = file_path + " not found.\n";
		throw runtime_error(outputstring.c_str());
		return;
	}

	string name;
	vector<Point> purePoints;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT2> UVs;
	vector<XMINT3> vertexIndices;

	model->vertexBufferSize = vertexIndices.size();
	infile.close();

	model->vertexBuffer = CreateVertexBuffer(&purePoints, &normals, &UVs, &vertexIndices);
	models.push_back(model);
}

void AssetManager::CreateRenderObject(int modelID, int diffuseID, int specularID)
{
	RenderObject* renderObject = new RenderObject();
	renderObject->model = models[modelID];

	if (diffuseID!=-1)
		renderObject->diffuseTexture = textures[diffuseID];
	if (specularID != -1)
		renderObject->specularTexture = textures[specularID];

	renderObjects.push_back(renderObject);
}

void AssetManager::LoadTexture(string file_path)
{
	ID3D11ShaderResourceView* texture;
	wstring widestr = wstring(file_path.begin(), file_path.end());
	DirectX::CreateWICTextureFromFile(device, widestr.c_str(), nullptr, &texture, 0);
	textures.push_back(texture);
}

ID3D11Buffer* AssetManager::CreateVertexBuffer(vector<Point>* purePoints, vector<XMFLOAT3>* normals, vector<XMFLOAT2>* UVs, vector<XMINT3>* vertexIndices)
{
	vector<Vertex> vertices;

	for (int i = 0; i < (signed)vertexIndices->size(); i += 3) 
	{
		for (int a = 0; a < 3; a++) 
		{
			Vertex tempVertex;
			tempVertex.position = purePoints->at(vertexIndices->at(i + a).x).position;
			tempVertex.normal = normals->at(vertexIndices->at(i + a).y);
			tempVertex.uv = UVs->at(vertexIndices->at(i + a).z);
			vertices.push_back(tempVertex);
		}
	}

	D3D11_BUFFER_DESC vbDESC;
	vbDESC.Usage = D3D11_USAGE_DEFAULT;

	vbDESC.ByteWidth = sizeof(Vertex)* vertexIndices->size();
	vbDESC.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDESC.CPUAccessFlags = 0;
	vbDESC.MiscFlags = 0;
	vbDESC.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;

	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	ID3D11Buffer* vertexBuffer;

	HRESULT result = device->CreateBuffer(&vbDESC, &vertexData, &vertexBuffer);
	if (FAILED(result)){
		throw runtime_error("Failed to create vertexBuffer");
		return nullptr;
	}

	return vertexBuffer;
}


RenderObject* AssetManager::GetRenderObject(int id)
{
	return renderObjects[id];
}