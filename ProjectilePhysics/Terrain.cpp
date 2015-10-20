#include "Terrain.h"
#include <iostream>

using namespace DirectX;
using namespace std;

Terrain::Terrain(ID3D11Device* device, char* heightMapName, float normalizeFactor, float sizeFactor, ID3D11ShaderResourceView* blendMap, ID3D11ShaderResourceView* grass, ID3D11ShaderResourceView* stone, ID3D11ShaderResourceView* sand)
{
	indexBuffer = nullptr;
	vertexBuffer = nullptr;
	vertexCount = 0;
	indexCount = 0;

	bool result = true;

	textureArray[0] = blendMap;
	textureArray[1] = grass;
	textureArray[2] = stone;
	textureArray[3] = sand;

	//Load, normalize, calculate normals and calculate texture coordinates for the heightmap
	result = LoadHeightMap(heightMapName);
	if (!result)
		throw runtime_error("Terrain: LoadHeightMap Error");

	this->sizeFactor = sizeFactor;
	EnlargeTerrain(sizeFactor);
	NormalizeHeightMap(normalizeFactor);

	CalculateNormals();
	CalculateTextureCoordinates();

	//Initialize the vertex and index buffer that holds the geometry for the terrain.
	InitializeBuffers(device);
}
Terrain::~Terrain()
{
	indexBuffer->Release();
	vertexBuffer->Release();

	for (auto t : textureArray)
		t->Release();

	delete[] heightMap;
}

int Terrain::GetVertexCount()
{
	return vertexCount;
}

void Terrain::GetBuffers(ID3D11Buffer*& vertexBuffer, ID3D11Buffer*& indexBuffer)
{
	vertexBuffer = this->vertexBuffer;
	indexBuffer = this->indexBuffer;
}

float Terrain::GetY(float x, float z)
{
	float returnValue = 0.0f;
	x /= sizeFactor;
	z /= sizeFactor;

	//Normalizing with bilinear interpolation
	if (x <= terrainWidth - 2 && z <= terrainHeight - 2 && x >= 0 + 1 && z >= 0 + 1)
	{
		int x1, x2, z1, z2;
		float q11, q12, q21, q22;
		x1 = (int)floor(x);
		x2 = (int)floor(x + 1);
		z1 = (int)floor(z);
		z2 = (int)floor(z + 1);

		q11 = GetHeightAt(x1, z1);
		q12 = GetHeightAt(x1, z2);
		q21 = GetHeightAt(x2, z1);
		q22 = GetHeightAt(x2, z2);

		returnValue = (1.0f / ((x2 - x1)*(z2 - z1)))*(q11*(x2 - x)*(z2 - z) +
			q21*(x - x1)*(z2 - z) +
			q12*(x2 - x)*(z - z1) +
			q22*(x - x1)*(z - z1));
	}
	return returnValue;
}

float Terrain::GetHeightAt(int x, int z)
{
	return heightMap[(terrainHeight * z) + x].y;
}

XMFLOAT3 Terrain::GetNormalAt(float x, float z)
{
	XMFLOAT3 returnValue(0, 0, 0);

	if (x <= terrainWidth - 2 && z <= terrainHeight - 2 && x >= 0 + 1 && z >= 0 + 1)
	{
		int x1, x2, z1, z2;
		float y11, y12, y21, y22;
		x1 = (int)floor(x);
		x2 = (int)floor(x + 1);
		z1 = (int)floor(z);
		z2 = (int)floor(z + 1);

		y11 = GetHeightAt(x1, z1);
		y12 = GetHeightAt(x1, z2);
		y21 = GetHeightAt(x2, z1);
		y22 = GetHeightAt(x2, z2);

		XMVECTOR vec1 = XMVector3Normalize(XMLoadFloat3(&XMFLOAT3(float(x2 - x1), y21 - y11, float(z1 - z1))));
		XMVECTOR vec2 = XMVector3Normalize(XMLoadFloat3(&XMFLOAT3(float(x1 - x1), y12 - y11, float(z2 - z1))));

		XMVECTOR n = XMVector3Normalize(XMVector3Cross(vec2, vec1));

		returnValue = XMFLOAT3(XMVectorGetX(n), XMVectorGetY(n), XMVectorGetZ(n));
	}

	return returnValue;
}

ID3D11ShaderResourceView** Terrain::GetTextures()
{
	return textureArray;
}

bool Terrain::LoadHeightMap(char* filename)
{
	FILE* filePtr;
	int error;
	unsigned int count;
	BITMAPFILEHEADER bitmapFileHeader = BITMAPFILEHEADER();
	BITMAPINFOHEADER bitmapInfoHeader = BITMAPINFOHEADER();
	int imageSize;
	unsigned char* bitmapImage;
	unsigned char height;

	// Open the height map file in binary
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
		throw runtime_error("Terrain(LoadHeightMap): Error opening file");

	//Read file and info headers
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
		throw runtime_error("Terrain(LoadHeightMap): Error reading file headers");

	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
		throw runtime_error("Terrain(LoadHeightMap): Error reading file headers");


	//Allocate the memory needed for the heightmap 
	terrainWidth = bitmapInfoHeader.biWidth;
	terrainHeight = bitmapInfoHeader.biHeight;
	imageSize = terrainWidth * terrainHeight * 3;
	bitmapImage = new unsigned char[imageSize];

	if (!bitmapImage)
		throw runtime_error("Terrain(LoadHeightMap): Error allocating memory for the bitmap image");

	//Read the file
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
		throw runtime_error("Terrain(LoadHeightMap): Error reading file");

	//Close the file
	error = fclose(filePtr);
	if(error != 0)
		throw runtime_error("Terrain(LoadHeightMap): Error closing file");

	heightMap = new HeightMap[terrainWidth * terrainHeight];
	if (!heightMap)
		throw runtime_error("Terrain(LoadHeightMap): Error creating HeightMap");

	int k = 0;
	int index = 0;

	//Read the image data into the height map
	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
		{
			height = bitmapImage[k];

			index = (terrainHeight * j) + i;

			heightMap[index].x = (float)i;
			heightMap[index].y = (float)height;
			heightMap[index].z = (float)j;

			k += 3;
		}
	}

	delete[] bitmapImage;
	bitmapImage = nullptr;

	return true;
}

void Terrain::NormalizeHeightMap(float factor)
{
	//Simple normalizing by a given number
	for (int i = 0; i < terrainHeight; i++)
	{
		for (int j = 0; j < terrainWidth; j++)
		{
			heightMap[(terrainHeight * i) + j].y /= factor;
		}
	}
}

void Terrain::EnlargeTerrain(float sizeFactor)
{
	for (int i = 0; i < terrainHeight; i++)
	{
		for (int j = 0; j < terrainWidth; j++)
		{
			heightMap[(terrainHeight * i) + j].y *= sizeFactor;
			heightMap[(terrainHeight * i) + j].x *= sizeFactor;
			heightMap[(terrainHeight * i) + j].z *= sizeFactor;
		}
	}
}

void Terrain::CalculateNormals()
{
	int index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;

	// Create a temporary array to hold the un-normalized normal vectors.
	float3* normals = new float3[(terrainHeight - 1) * (terrainWidth - 1)];

	// Go through all the faces in the mesh and calculate their normals.
	for (int j = 0; j < (terrainHeight - 1); j++)
	{
		for (int i = 0; i < (terrainWidth - 1); i++)
		{
			index1 = (j * terrainHeight) + i;
			index2 = (j * terrainHeight) + (i + 1);
			index3 = ((j + 1) * terrainHeight) + i;

			//Get three vertices from the face.
			vertex1[0] = heightMap[index1].x;
			vertex1[1] = heightMap[index1].y;
			vertex1[2] = heightMap[index1].z;

			vertex2[0] = heightMap[index2].x;
			vertex2[1] = heightMap[index2].y;
			vertex2[2] = heightMap[index2].z;

			vertex3[0] = heightMap[index3].x;
			vertex3[1] = heightMap[index3].y;
			vertex3[2] = heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (terrainHeight - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	//Go through all the vertices and take an average of each face normal 	
	//that the vertex touches to get the averaged normal for that vertex.
	for (int j = 0; j < terrainHeight; j++)
	{
		for (int i = 0; i < terrainWidth; i++)
		{
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;
			count = 0;

			// Bottom left face
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face
			if ((i < (terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face
			if (((i - 1) >= 0) && (j < (terrainHeight - 1)))
			{
				index = (j * (terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face
			if ((i < (terrainWidth - 1)) && (j < (terrainHeight - 1)))
			{
				index = (j * (terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			//Average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);


			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			//index to the vertex location in the height map array
			index = (j * terrainHeight) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			heightMap[index].nx = (sum[0] / length);
			heightMap[index].ny = (sum[1] / length);
			heightMap[index].nz = (sum[2] / length);
		}
	}

	delete[] normals;
	normals = nullptr;
}

void Terrain::CalculateTextureCoordinates()
{
	float incrementValue = (float)TEXTURE_REPEAT / (float)terrainWidth;
	int incrementCount = terrainWidth / TEXTURE_REPEAT;
	float tuCoordinate = 0.0f;
	float tvCoordinate = 1.0f;
	int tuCount = 0;
	int tvCount = 0;

	//Loop through heightmap and calculate texture coordinates for each vertex
	for (int i = 0; i < terrainHeight; i++)
	{
		for (int j = 0; j < terrainWidth; j++)
		{
			//Store the texture coordinate in the height map
			heightMap[(terrainHeight * i) + j].tu = tuCoordinate;
			heightMap[(terrainHeight * i) + j].tv = tvCoordinate;

			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture. If so, start from the left
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture. If so, start from the bottom
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}
}

void Terrain::InitializeBuffers(ID3D11Device* device)
{
	HRESULT result;
	int index1, index2, index3, index4;

	vertexCount = (terrainWidth - 1) * (terrainHeight - 1) * 6;
	indexCount = vertexCount;

	Vertex* vertices = new Vertex[vertexCount];
	unsigned long* indices = new unsigned long[indexCount];

	int index = 0;

	float tu = 0.0f;
	float tv = 0.0f;

	//Load the vertex and index array with the terrain data.
	for (int i = 0; i < (terrainHeight - 1); i++)
	{
		for (int j = 0; j < (terrainWidth - 1); j++)
		{
			index1 = (terrainHeight * i) + j;					//Bottom left
			index2 = (terrainHeight * i) + (j + 1);				//Bottom right
			index3 = (terrainHeight * (i + 1)) + j;				//Upper left
			index4 = (terrainHeight * (i + 1)) + (j + 1);		//Upper right

			tv = heightMap[index3].tv;
			//Cover the top edge
			if (tv == 1.0f)
				tv = 0.0f;

			vertices[index].position = XMFLOAT3(heightMap[index3].x, heightMap[index3].y, heightMap[index3].z);
			vertices[index].uv = XMFLOAT2(heightMap[index3].tu, tv);
			vertices[index].normal = XMFLOAT3(heightMap[index3].nx, heightMap[index3].ny, heightMap[index3].nz);
			indices[index] = index;
			index++;

			// Upper right
			tu = heightMap[index4].tu;
			tv = heightMap[index4].tv;

			//Cover the top and right edge
			if (tu == 0.0f)
				tu = 1.0f;
			if (tv == 1.0f)
				tv = 0.0f;

			vertices[index].position = XMFLOAT3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
			vertices[index].uv = XMFLOAT2(tu, tv);
			vertices[index].normal = XMFLOAT3(heightMap[index4].nx, heightMap[index4].ny, heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom left
			vertices[index].position = XMFLOAT3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
			vertices[index].uv = XMFLOAT2(heightMap[index1].tu, heightMap[index1].tv);
			vertices[index].normal = XMFLOAT3(heightMap[index1].nx, heightMap[index1].ny, heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Bottom left
			vertices[index].position = XMFLOAT3(heightMap[index1].x, heightMap[index1].y, heightMap[index1].z);
			vertices[index].uv = XMFLOAT2(heightMap[index1].tu, heightMap[index1].tv);
			vertices[index].normal = XMFLOAT3(heightMap[index1].nx, heightMap[index1].ny, heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Upper right
			tu = heightMap[index4].tu;
			tv = heightMap[index4].tv;

			//Cover the top and right edge.
			if (tu == 0.0f)
				tu = 1.0f;
			if (tv == 1.0f)
				tv = 0.0f;

			vertices[index].position = XMFLOAT3(heightMap[index4].x, heightMap[index4].y, heightMap[index4].z);
			vertices[index].uv = XMFLOAT2(tu, tv);
			vertices[index].normal = XMFLOAT3(heightMap[index4].nx, heightMap[index4].ny, heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom right
			tu = heightMap[index2].tu;

			// Cover the right edge.
			if (tu == 0.0f)
				tu = 1.0f;

			vertices[index].position = XMFLOAT3(heightMap[index2].x, heightMap[index2].y, heightMap[index2].z);
			vertices[index].uv = XMFLOAT2(tu, heightMap[index2].tv);
			vertices[index].normal = XMFLOAT3(heightMap[index2].nx, heightMap[index2].ny, heightMap[index2].nz);
			indices[index] = index;
			index++;
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;

	//Description of the vertex buffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//Create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;

	//Description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	delete[] vertices;
	vertices = nullptr;
	delete[] indices;
	indices = nullptr;
}