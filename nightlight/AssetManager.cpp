#include "AssetManager.h"

AssetManager::AssetManager(ID3D11Device* device)
{
	this->device = device;

	vector<string> modelNames;
	fileToStrings("Assets/models.txt", modelNames);
	for (int i = 0; i < (signed)modelNames.size(); i++)
		LoadModel("Assets/Models/" + modelNames[i]);

	vector<string> textureNames;
	fileToStrings("Assets/textures.txt", textureNames);
	for (int i = 0; i < (signed)textureNames.size(); i++)
		LoadTexture("Assets/Textures/" + textureNames[i]);

	vector<string> renderObjectIDs;
	fileToStrings("Assets/renderObjects.txt", renderObjectIDs);
	for (int i = 0; i < (signed)renderObjectIDs.size(); i++)
	{
		vector<int> IDs = stringToIntArray(renderObjectIDs[i]);
		CreateRenderObject(IDs[0], IDs[1], IDs[2]);
	}
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

void AssetManager::LoadModel(string file_path){
	Model* model = new Model();

	ifstream infile;
	infile.open(file_path.c_str(), ifstream::binary);
	if (!infile.is_open())
	{
		string outputstring = file_path + " not found.\n";
		throw runtime_error(outputstring.c_str());
		return;
	}
	MainHeader mainHeader;
	infile.read((char*)&mainHeader, sizeof(MainHeader));

	string name;
	vector<WeightedPoint> points;
	vector<Point> purePoints;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT2> UVs;
	vector<XMINT3> vertexIndices;

	for (int i = 0; i < mainHeader.meshCount; i++){
		if (i == 0){
			MeshHeader meshHeader;
			infile.read((char*)&meshHeader, sizeof(MeshHeader));


			name.resize(meshHeader.nameLength);
			if (meshHeader.hasSkeleton)
				points.resize(meshHeader.numberPoints);
			else
				purePoints.resize(meshHeader.numberPoints);
			normals.resize(meshHeader.numberNormals);
			UVs.resize(meshHeader.numberCoords);
			vertexIndices.resize(meshHeader.numberFaces * 3);
			model->hasSkeleton = meshHeader.hasSkeleton;


			infile.read((char*)name.data(), meshHeader.nameLength);
			model->name = name;
			if (meshHeader.hasSkeleton)
				infile.read((char*)points.data(), meshHeader.numberPoints*sizeof(WeightedPoint));
			else
				infile.read((char*)purePoints.data(), meshHeader.numberPoints*sizeof(Point));
			infile.read((char*)normals.data(), meshHeader.numberNormals*sizeof(XMFLOAT3));
			infile.read((char*)UVs.data(), meshHeader.numberCoords*sizeof(XMFLOAT2));
			infile.read((char*)vertexIndices.data(), meshHeader.numberFaces*sizeof(XMINT3) * 3);
		}
		else{
			MeshHeader meshHeader;
			infile.read((char*)&meshHeader, sizeof(MeshHeader));
			if (meshHeader.hasSkeleton)
			{
				infile.seekg(meshHeader.nameLength, ios::cur);
				infile.seekg(meshHeader.numberPoints*sizeof(WeightedPoint), ios::cur);
				infile.seekg(meshHeader.numberNormals*sizeof(XMFLOAT3), ios::cur);
				infile.seekg(meshHeader.numberCoords*sizeof(XMFLOAT2), ios::cur);
				infile.seekg(meshHeader.numberFaces*sizeof(XMINT3) * 3, ios::cur);
			}
			else
			{
				infile.seekg(meshHeader.nameLength, ios::cur);
				infile.seekg(meshHeader.numberPoints*sizeof(Point), ios::cur);
				infile.seekg(meshHeader.numberNormals*sizeof(XMFLOAT3), ios::cur);
				infile.seekg(meshHeader.numberCoords*sizeof(XMFLOAT2), ios::cur);
				infile.seekg(meshHeader.numberFaces*sizeof(XMINT3) * 3, ios::cur);
			}
		}
	}


	for (int i = 0; i < mainHeader.matCount; i++)
	{
		if (i == 0){
			MatHeader matHeader;
			infile.read((char*)&matHeader, sizeof(MatHeader));

			infile.seekg(16 + matHeader.ambientNameLength, ios::cur);

			infile.read((char*)&model->diffuse, 16);
			infile.seekg(matHeader.diffuseNameLength, ios::cur);

			infile.read((char*)&model->specular, 16);
			infile.seekg(matHeader.specularNameLength, ios::cur);

			infile.seekg(16 + matHeader.transparencyNameLength, ios::cur);

			infile.seekg(16 + matHeader.glowNameLength, ios::cur);
		}

		else{
			MatHeader matHeader;
			infile.read((char*)&matHeader, sizeof(MatHeader));
			infile.seekg(80
				+ matHeader.ambientNameLength
				+ matHeader.diffuseNameLength
				+ matHeader.specularNameLength
				+ matHeader.transparencyNameLength
				+ matHeader.glowNameLength,
				ios::cur);
		}
	}

	model->pointLights.resize(mainHeader.pointLightSize);

	if (mainHeader.ambientLightSize)
		infile.seekg(mainHeader.ambientLightSize*sizeof(AmbientLightStruct), ios::cur);
	if (mainHeader.areaLightSize)
		infile.seekg(mainHeader.areaLightSize*sizeof(AreaLightStruct), ios::cur);
	if (mainHeader.dirLightSize)
		infile.seekg(mainHeader.dirLightSize* sizeof(DirectionalLightStruct), ios::cur);
	if (mainHeader.pointLightSize)
		infile.read((char*)model->pointLights.data(), mainHeader.pointLightSize* sizeof(PointLightStruct));
	if (mainHeader.spotLightSize){
		infile.read((char*)&model->spotLight, sizeof(SpotLightStruct));
		infile.seekg(mainHeader.spotLightSize - 1 * sizeof(SpotLightStruct), ios::cur);
	}

	infile.seekg(mainHeader.camCount * 52, ios::cur);

/*	
	for (int i = 0; i < mainHeader.camCount; i++){
		läs kameror
	}
*/
	if (mainHeader.blendShapeCount==3)
		model->hasBlendShapes = true;
	
	std::vector<BlendShape> blendShapes;
	for (int i = 0; i < mainHeader.blendShapeCount; i++){
		BlendShape blendShape;
		infile.read((char*)&blendShape.MeshTarget, 4);
		blendShape.points.resize(points.size() + purePoints.size());
		infile.read((char*)blendShape.points.data(), blendShape.points.size()*sizeof(XMFLOAT3));
		blendShape.normals.resize(points.size() + purePoints.size());
		infile.read((char*)blendShape.normals.data(), blendShape.normals.size()*sizeof(XMFLOAT3));
		

		int frames;
		infile.read((char*)&frames, 4);
		infile.seekg(8*frames,std::ios::cur);

		
		blendShapes.push_back(blendShape);
	}

	model->skeleton.resize(mainHeader.boneCount);

	for (int i = 0; i < mainHeader.boneCount; i++){
		infile.read((char*)&model->skeleton[i].parent, 4);
		infile.read((char*)&model->skeleton[i].invBindPose, 64);

		int frames;
		infile.read((char*)&frames, 4);

		model->skeleton[i].frames.resize(frames);

		if (frames)
		infile.read((char*)model->skeleton[i].frames.data(), sizeof(Keyframe)*frames);
	}

//	if (mainHeader.boneCount)
//		setUpBones(model);

	model->vertexBufferSize = vertexIndices.size();
	infile.close();

	model->vertexBuffer = CreateVertexBuffer(&points, &purePoints, &normals, &UVs, &vertexIndices, model->hasSkeleton, &blendShapes);
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

ID3D11Buffer* AssetManager::CreateVertexBuffer(vector<WeightedPoint> *points, vector<Point> *purePoints, vector<XMFLOAT3> *normals, vector<XMFLOAT2> *UVs, vector<XMINT3> *vertexIndices, bool hasSkeleton, std::vector<BlendShape> *blendShapes)
{
	vector<WeightedVertex> weightedVertices;
	vector<Vertex> vertices;
	vector<WeightedBlendVertex> weightedBlendVertices;
	vector<BlendVertex> blendVertices;

	if (hasSkeleton){
		if (blendShapes->size() > 0){
			for (int i = 0; i < (signed)vertexIndices->size(); i += 3){
				for (int a = 0; a < 3; a++){
					WeightedBlendVertex tempVertex;
					tempVertex.position0 = points->at(vertexIndices->at(i + a).x).position;
					tempVertex.position1 = blendShapes->at(0).points[vertexIndices->at(i + a).x];
					tempVertex.position2 = blendShapes->at(1).points[vertexIndices->at(i + a).x];
					tempVertex.position3 = blendShapes->at(2).points[vertexIndices->at(i + a).x];

					tempVertex.normal0 = normals->at(vertexIndices->at(i + a).y);
					if (vertexIndices->at(i + a).y < (signed)blendShapes->at(0).normals.size()){
						tempVertex.normal1 = blendShapes->at(0).normals[vertexIndices->at(i + a).y];
						tempVertex.normal2 = blendShapes->at(1).normals[vertexIndices->at(i + a).y];
						tempVertex.normal3 = blendShapes->at(2).normals[vertexIndices->at(i + a).y];
					}
					else{
						tempVertex.normal1 = normals->at(vertexIndices->at(i + a).y);
						tempVertex.normal2 = normals->at(vertexIndices->at(i + a).y);
						tempVertex.normal3 = normals->at(vertexIndices->at(i + a).y);
					}

					tempVertex.uv = UVs->at(vertexIndices->at(i + a).z);
					for (int b = 0; b < 4; b++)
					{
						tempVertex.boneIndices[b] = points->at(vertexIndices->at(i + a).x).boneIndices[b];
						tempVertex.boneWeigths[b] = points->at(vertexIndices->at(i + a).x).boneWeigths[b];
					}
					weightedBlendVertices.push_back(tempVertex);
				}
			}
		}
		else{
			for (int i = 0; i < (signed)vertexIndices->size(); i += 3){
				for (int a = 0; a < 3; a++){
					WeightedVertex tempVertex;
					tempVertex.position = points->at(vertexIndices->at(i + a).x).position;
					tempVertex.normal = normals->at(vertexIndices->at(i + a).y);
					tempVertex.uv = UVs->at(vertexIndices->at(i + a).z);
					for (int b = 0; b < 4; b++)
					{
						tempVertex.boneIndices[b] = points->at(vertexIndices->at(i + a).x).boneIndices[b];
						tempVertex.boneWeigths[b] = points->at(vertexIndices->at(i + a).x).boneWeigths[b];
					}
					weightedVertices.push_back(tempVertex);
				}
			}
		}
	}
	else{
		if (blendShapes->size() > 0){
			for (int i = 0; i < (signed)vertexIndices->size(); i += 3){
				for (int a = 0; a < 3; a++){
					BlendVertex tempVertex;

					tempVertex.position0 = purePoints->at(vertexIndices->at(i + a).x).position;
					tempVertex.position1 = blendShapes->at(0).points[vertexIndices->at(i + a).x];
					tempVertex.position2 = blendShapes->at(1).points[vertexIndices->at(i + a).x];
					tempVertex.position3 = blendShapes->at(2).points[vertexIndices->at(i + a).x];
					tempVertex.normal0 = normals->at(vertexIndices->at(i + a).y);

					tempVertex.normal1 = blendShapes->at(0).normals[vertexIndices->at(i + a).x];
					tempVertex.normal2 = blendShapes->at(1).normals[vertexIndices->at(i + a).x];
					tempVertex.normal3 = blendShapes->at(2).normals[vertexIndices->at(i + a).x];
					tempVertex.uv = UVs->at(vertexIndices->at(i + a).z);

					blendVertices.push_back(tempVertex);
				}
			}
		}
		else{
			for (int i = 0; i < (signed)vertexIndices->size(); i += 3){
				for (int a = 0; a < 3; a++){
					Vertex tempVertex;
					tempVertex.position = purePoints->at(vertexIndices->at(i + a).x).position;
					tempVertex.normal = normals->at(vertexIndices->at(i + a).y);
					tempVertex.uv = UVs->at(vertexIndices->at(i + a).z);
					vertices.push_back(tempVertex);
				}
			}
		}
	}

	D3D11_BUFFER_DESC vbDESC;
	vbDESC.Usage = D3D11_USAGE_DEFAULT;
	if (hasSkeleton)
		if (blendShapes->size() > 0)
			vbDESC.ByteWidth = sizeof(WeightedBlendVertex)* vertexIndices->size();
		else
			vbDESC.ByteWidth = sizeof(WeightedVertex)* vertexIndices->size();
	else
		if (blendShapes->size() > 0)
			vbDESC.ByteWidth = sizeof(BlendVertex)* vertexIndices->size();
		else
			vbDESC.ByteWidth = sizeof(Vertex)* vertexIndices->size();
	vbDESC.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDESC.CPUAccessFlags = 0;
	vbDESC.MiscFlags = 0;
	vbDESC.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;

	if (hasSkeleton)
		if (blendShapes->size() > 0)
			vertexData.pSysMem = weightedBlendVertices.data();
		else
			vertexData.pSysMem = weightedVertices.data();
	else
		if (blendShapes->size() > 0)
			vertexData.pSysMem = blendVertices.data();
		else
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

void AssetManager::setUpBones(Model* model)
{
	int currentFrame = 0, finalFrame = model->skeleton[0].frames.size();

	std::vector<XMFLOAT4X4> boneLocalMatrices;
	std::vector<XMFLOAT4X4> boneGlobalMatrices;
	std::vector<Bone>* bones = &model->skeleton;
	boneGlobalMatrices.resize(model->skeleton.size());
	boneLocalMatrices.resize(model->skeleton.size());
	XMFLOAT3 idnt(1, 1, 1);
	XMVECTOR det;
	XMVECTOR S = XMLoadFloat3(&idnt);
	XMVECTOR P = XMLoadFloat3(&bones->at(0).frames[currentFrame].trans);
	XMVECTOR Q = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&bones->at(0).frames[currentFrame].rot));

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&boneLocalMatrices[0], XMMatrixAffineTransformation(S, zero, Q, P));

	boneGlobalMatrices[0] = boneLocalMatrices[0];
	XMStoreFloat4x4(&bones->at(0).invBindPose, XMMatrixInverse(&det, XMLoadFloat4x4(&boneGlobalMatrices[0])));
	for (int i = 1; i < (signed)boneGlobalMatrices.size(); i++)
	{
		if (currentFrame < finalFrame){

			XMVECTOR s0 = XMLoadFloat3(&idnt);
			XMVECTOR trans = XMLoadFloat3(&bones->at(i).frames[currentFrame].trans);
			XMVECTOR rot = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&bones->at(i).frames[currentFrame].rot));

			XMStoreFloat4x4(&boneLocalMatrices[i], XMMatrixAffineTransformation(S, zero, rot, trans));
			
		}
		XMMATRIX local = XMLoadFloat4x4(&boneLocalMatrices[i]);
		XMMATRIX global = XMLoadFloat4x4(&boneGlobalMatrices[bones->at(i).parent]);
		XMMATRIX toRoot = XMMatrixMultiply(local, global);

		XMStoreFloat4x4(&bones->at(i).invBindPose, XMMatrixInverse(&det, toRoot));
		XMStoreFloat4x4(&boneGlobalMatrices[i], toRoot);
	}
}
