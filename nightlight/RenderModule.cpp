#include "RenderModule.h"
#include <fstream>
#include <string>
using namespace std;


RenderModule::RenderModule(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen, int shadowMapSize)
{
	vertexShader = NULL;
	pixelShader = NULL;
	skeletalVertexShader = NULL;
	blendVertexShader = NULL;
	sampleStateWrap = NULL;
	sampleStateClamp = NULL;
	matrixBufferPerObject = NULL;
	matrixBufferPerWeightedObject = NULL;
	matrixBufferPerBlendObject = NULL;
	matrixBufferPerFrame = NULL;
	lightBuffer = NULL;
	this->hwnd = hwnd;

	d3d = new D3DManager(hwnd, screenWidth, screenHeight, fullscreen);
	shadowMap = new ShadowMap(d3d->GetDevice(), shadowMapSize, L"Assets/Shaders/ShadowVS.hlsl");

	bool result;

	InitializeSamplers();
	InitializeConstantBuffers();

	result = InitializeShader(L"Assets/Shaders/vertexShader.hlsl", L"Assets/Shaders/pixelShader.hlsl");
	result = InitializeSkeletalShader(L"Assets/Shaders/skeletalVertexShader.hlsl", L"Assets/Shaders/pixelShader.hlsl");
	result = InitializeBlendShader(L"Assets/Shaders/blendVertexShader.hlsl", L"Assets/Shaders/pixelShader.hlsl");
}

bool RenderModule::InitializeSamplers()
{
	D3D11_SAMPLER_DESC samplerDesc;
	HRESULT result;
	ID3D11Device* device = d3d->GetDevice();

	//Create a WRAP texture sampler state description
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &sampleStateWrap);
	if (FAILED(result))
		throw std::runtime_error("RenderModule: samplerStateWrap initialization failed.");

	//Create a CLAMP texture sampler state description.
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = device->CreateSamplerState(&samplerDesc, &sampleStateClamp);
	if (FAILED(result))
		throw std::runtime_error("RenderModule: samplerStateClamp initialization failed.");

	//Create a COMPARISON sampler state
	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = 0.f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.MipLODBias = 0.f;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

	result = device->CreateSamplerState(&samplerDesc, &sampleStateComparison);
	if (FAILED(result))
		throw std::runtime_error("RenderModule: samplerStateComparison initialization failed.");

	return true;
}

bool RenderModule::InitializeConstantBuffers()
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	HRESULT result;
	ID3D11Device* device = d3d->GetDevice();

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerObject);
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBufferPerObject);

	if (FAILED(result))
		throw std::runtime_error("RenderModule: Failed to create MatrixBufferPerObject");

	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerFrame);
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBufferPerFrame);

	if (FAILED(result))
		throw std::runtime_error("RenderModule: Failed to create MatrixBufferPerFrame");

	matrixBufferDesc.ByteWidth = sizeof(LightBuffer);
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &lightBuffer);

	if (FAILED(result))
		throw std::runtime_error("RenderModule: Failed to create LightBuffer");

	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerWeightedObject);
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBufferPerWeightedObject);

	if (FAILED(result))
		throw std::runtime_error("RenderModule: Failed to create MatrixBufferPerWeightedObject");

	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerBlendObject);
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBufferPerBlendObject);

	if (FAILED(result))
		throw std::runtime_error("RenderModule: Failed to create MatrixBufferPerBlendObject");

	return true;
}

RenderModule::~RenderModule()
{
	delete d3d;
	delete shadowMap;

	layoutPosUvNorm->Release();
	layoutPosUvNormIdxWei->Release();
	layoutPosUvNorm3PosNorm->Release();

	matrixBufferPerObject->Release();
	matrixBufferPerWeightedObject->Release();
	matrixBufferPerBlendObject->Release();
	matrixBufferPerFrame->Release();

	pixelShader->Release();
	vertexShader->Release();
	skeletalVertexShader->Release();
	blendVertexShader->Release();

	sampleStateClamp->Release();
	sampleStateWrap->Release();
	sampleStateComparison->Release();
}

bool RenderModule::InitializeShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;

	ID3D11Device* device = d3d->GetDevice();

	/////////////////////////////////////////////////////////////////////////// Shaders ///////////////////////////////////////////////////////////////////////////
	if (!vertexShader)
	{
		result = D3DCompileFromFile(vsFilename, NULL, NULL, "vertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
				throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			else
				throw std::runtime_error("Vertexshader not found");;

			return false;
		}

		result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
		if (FAILED(result))
			return false;
	}

	if (!pixelShader)
	{
		result = D3DCompileFromFile(psFilename, NULL, NULL, "pixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
				throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			else
				throw std::runtime_error("Pixelshader not found");

			return false;
		}

		result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
		if (FAILED(result))
			return false;

	}

	/////////////////////////////////////////////////////////////////////// Input layout /////////////////////////////////////////////////////////////////////////
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layoutPosUvNorm);
	if (FAILED(result)){ return false; }

	if (vertexShaderBuffer)
	{
		vertexShaderBuffer->Release();
		vertexShaderBuffer = nullptr;
	}

	if (pixelShaderBuffer)
	{
		pixelShaderBuffer->Release();
		pixelShaderBuffer = nullptr;
	}


	return true;
}

bool RenderModule::InitializeSkeletalShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	ID3D11Device* device = d3d->GetDevice();

	/////////////////////////////////////////////////////////////////////////// Shaders ///////////////////////////////////////////////////////////////////////////
	if (!skeletalVertexShader)
	{
		result = D3DCompileFromFile(vsFilename, NULL, NULL, "skeletalVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
				throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			else
				throw std::runtime_error("Vertexshader not found");;

			return false;
		}

		result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &skeletalVertexShader);
		if (FAILED(result))
			return false;
	}


	if (!pixelShader)
	{
		result = D3DCompileFromFile(psFilename, NULL, NULL, "pixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
				throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			else
				throw std::runtime_error("Pixelshader not found");

			return false;
		}

		result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
		if (FAILED(result))
			return false;
	}

	/////////////////////////////////////////////////////////////////////// Input layout /////////////////////////////////////////////////////////////////////////
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "INDICES";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32A32_SINT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "WEIGHTS";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layoutPosUvNormIdxWei);
	if (FAILED(result)){ return false; }

	if (vertexShaderBuffer)
	{
		vertexShaderBuffer->Release();
		vertexShaderBuffer = nullptr;
	}

	if (pixelShaderBuffer)
	{
		pixelShaderBuffer->Release();
		pixelShaderBuffer = nullptr;
	}

	return true;

}

bool RenderModule::InitializeBlendShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[9];
	unsigned int numElements;

	ID3D11Device* device = d3d->GetDevice();

	/////////////////////////////////////////////////////////////////////////// Shaders ///////////////////////////////////////////////////////////////////////////
	if (!blendVertexShader)
	{
		result = D3DCompileFromFile(vsFilename, NULL, NULL, "blendVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
				throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			else
				throw std::runtime_error("\nBlendVertexshader not found");;

			return false;
		}

		result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &blendVertexShader);
		if (FAILED(result))
			return false;
	}

	if (!pixelShader)
	{
		result = D3DCompileFromFile(psFilename, NULL, NULL, "pixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
				throw runtime_error(string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			else
				throw std::runtime_error("\nPixelshader not found");

			return false;
		}

		result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
		if (FAILED(result))
			return false;
	}

	/////////////////////////////////////////////////////////////////////// Input layout /////////////////////////////////////////////////////////////////////////
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "POSITION";
	polygonLayout[3].SemanticIndex = 1;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "POSITION";
	polygonLayout[4].SemanticIndex = 2;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	polygonLayout[5].SemanticName = "POSITION";
	polygonLayout[5].SemanticIndex = 3;
	polygonLayout[5].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[5].InputSlot = 0;
	polygonLayout[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[5].InstanceDataStepRate = 0;

	polygonLayout[6].SemanticName = "NORMAL";
	polygonLayout[6].SemanticIndex = 1;
	polygonLayout[6].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[6].InputSlot = 0;
	polygonLayout[6].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[6].InstanceDataStepRate = 0;

	polygonLayout[7].SemanticName = "NORMAL";
	polygonLayout[7].SemanticIndex = 2;
	polygonLayout[7].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[7].InputSlot = 0;
	polygonLayout[7].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[7].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[7].InstanceDataStepRate = 0;

	polygonLayout[8].SemanticName = "NORMAL";
	polygonLayout[8].SemanticIndex = 3;
	polygonLayout[8].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[8].InputSlot = 0;
	polygonLayout[8].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[8].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[8].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layoutPosUvNorm3PosNorm);
	if (FAILED(result)){ return false; }

	if (vertexShaderBuffer)
	{
		vertexShaderBuffer->Release();
		vertexShaderBuffer = nullptr;
	}

	if (pixelShaderBuffer)
	{
		pixelShaderBuffer->Release();
		pixelShaderBuffer = nullptr;
	}

	return true;
}

bool RenderModule::SetDataPerObject(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 colorModifier)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();


	UINT32 vertexSize;
	if (renderObject->model->hasSkeleton)
		vertexSize = sizeof(WeightedVertex);
	else
		vertexSize = sizeof(Vertex);

	UINT32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &renderObject->model->vertexBuffer, &vertexSize, &offset);
	deviceContext->PSSetShaderResources(0, 1, &renderObject->diffuseTexture);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX worldMatrixC;
	worldMatrixC = XMMatrixTranspose(worldMatrix);

	result = deviceContext->Map(matrixBufferPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	MatrixBufferPerObject* dataPtr = (MatrixBufferPerObject*)mappedResource.pData;

	dataPtr->world = worldMatrixC;
	dataPtr->colorModifier = colorModifier;

	deviceContext->Unmap(matrixBufferPerObject, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &matrixBufferPerObject);
	return true;
}

bool RenderModule::SetDataPerBlendObject(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 colorModifier, XMFLOAT4& weights)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	UINT32 vertexSize;
	if (renderObject->model->hasSkeleton)
		vertexSize = sizeof(WeightedBlendVertex);
	else
		vertexSize = sizeof(BlendVertex);

	UINT32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &renderObject->model->vertexBuffer, &vertexSize, &offset);
	deviceContext->PSSetShaderResources(0, 1, &renderObject->diffuseTexture);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX worldMatrixC;
	worldMatrixC = XMMatrixTranspose(worldMatrix);

	result = deviceContext->Map(matrixBufferPerBlendObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	MatrixBufferPerBlendObject* dataPtr = (MatrixBufferPerBlendObject*)mappedResource.pData;

	dataPtr->world = worldMatrixC;
	dataPtr->weight[0] = weights.x;
	dataPtr->weight[1] = weights.y;
	dataPtr->weight[2] = weights.z;
	dataPtr->weight[3] = weights.w;
	dataPtr->colorModifier = colorModifier;

	deviceContext->Unmap(matrixBufferPerBlendObject, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &matrixBufferPerBlendObject);
	return true;
}

bool RenderModule::SetDataPerSkeletalObject(XMMATRIX& worldMatrix, RenderObject* renderObject, bool isSelected, float frame)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	UINT32 vertexSize;
	vertexSize = sizeof(WeightedVertex);

	UINT32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &renderObject->model->vertexBuffer, &vertexSize, &offset);
	deviceContext->PSSetShaderResources(0, 1, &renderObject->diffuseTexture);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX worldMatrixC;
	worldMatrixC = XMMatrixTranspose(worldMatrix);

	int currentFrame = (int)frame, finalFrame = renderObject->model->skeleton[0].frames.size();
	float interpolation = frame - (float)currentFrame;

	std::vector<XMFLOAT4X4> boneLocalMatrices;
	std::vector<XMFLOAT4X4> boneGlobalMatrices;
	std::vector<Bone>* bones = &renderObject->model->skeleton;
	boneGlobalMatrices.resize(renderObject->model->skeleton.size());
	boneLocalMatrices.resize(renderObject->model->skeleton.size());
	XMFLOAT3 idnt(1, 1, 1);
	XMVECTOR S = XMLoadFloat3(&idnt);
	XMVECTOR P = XMLoadFloat3(&bones->at(0).frames[currentFrame].trans);
	XMVECTOR Q = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&bones->at(0).frames[currentFrame].rot));

	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&boneLocalMatrices[0], XMMatrixAffineTransformation(S, zero, Q, P));

	XMStoreFloat4x4(&boneLocalMatrices[0], XMMatrixRotationQuaternion(Q)*XMMatrixTranslationFromVector(P));

	boneGlobalMatrices[0] = boneLocalMatrices[0];
	for (int i = 1; i < (signed)boneGlobalMatrices.size(); i++)
	{
		if (currentFrame < finalFrame){

			XMVECTOR s0 = XMLoadFloat3(&idnt);
			XMVECTOR trans = XMLoadFloat3(&bones->at(i).frames[currentFrame].trans);
			XMVECTOR rot = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&bones->at(i).frames[currentFrame].rot));

			if (currentFrame + 1 < finalFrame){

				XMVECTOR s1 = XMLoadFloat3(&idnt);
				XMVECTOR trans2 = XMLoadFloat3(&bones->at(i).frames[currentFrame + 1].trans);
				XMVECTOR rot2 = XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&bones->at(i).frames[currentFrame + 1].rot));

				S = XMVectorLerp(s0, s1, interpolation);
				P = XMVectorLerp(trans, trans2, interpolation);
				Q = XMQuaternionSlerp(rot, rot2, interpolation);

				XMStoreFloat4x4(&boneLocalMatrices[i], XMMatrixAffineTransformation(S, zero, Q, P));
			}
			else
			{
				XMStoreFloat4x4(&boneLocalMatrices[i], XMMatrixAffineTransformation(S, zero, rot, trans));
			}
		}
		XMMATRIX local = XMLoadFloat4x4(&boneLocalMatrices[i]);
		XMMATRIX global = XMLoadFloat4x4(&boneGlobalMatrices[bones->at(i).parent]);
		XMMATRIX toRoot = XMMatrixMultiply(local, global);

		XMStoreFloat4x4(&boneGlobalMatrices[i], toRoot);
	}

	result = deviceContext->Map(matrixBufferPerWeightedObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }
	MatrixBufferPerWeightedObject* dataPtr = (MatrixBufferPerWeightedObject*)mappedResource.pData;
	dataPtr->world = worldMatrixC;

	std::vector<XMFLOAT4X4> finalTransforms;
	finalTransforms.resize(boneGlobalMatrices.size());
	for (int i = 0; i < (signed)boneGlobalMatrices.size(); i++)
	{
		XMMATRIX offset = XMLoadFloat4x4(&bones->at(i).invBindPose);
		XMMATRIX toRoot = XMLoadFloat4x4(&boneGlobalMatrices[i]);

		XMStoreFloat4x4(&finalTransforms[i], XMMatrixMultiply(offset, toRoot));

		dataPtr->bones[i] = finalTransforms[i];
		//XMStoreFloat4x4(&dataPtr->bones[i], XMMatrixTranslation(0, 0, 0));
	}

	deviceContext->Unmap(matrixBufferPerWeightedObject, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBufferPerWeightedObject);

	return true;
}

bool RenderModule::SetDataPerFrame(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, XMFLOAT3& camPos, LightObject * spotlight, LevelStates* levelstate)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferPerFrame* dataPtr;
	LightBuffer* lightPtr;
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	//View,Projection
	XMMATRIX viewMatrixC, projectionMatrixC;

	viewMatrixC = XMMatrixTranspose(viewMatrix);
	projectionMatrixC = XMMatrixTranspose(projectionMatrix);

	result = deviceContext->Map(matrixBufferPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	dataPtr = (MatrixBufferPerFrame*)mappedResource.pData;

	dataPtr->viewMatrix = viewMatrixC;
	dataPtr->projectionMatrix = projectionMatrixC;
	dataPtr->camPos = camPos;

	deviceContext->Unmap(matrixBufferPerFrame, 0);

	deviceContext->VSSetConstantBuffers(1, 1, &matrixBufferPerFrame);
	
	//Light
	XMMATRIX lvt, lpt;
	spotlight->getViewMatrix(lvt);
	spotlight->getProjMatrix(lpt);

	lvt = XMMatrixTranspose(lvt);
	lpt = XMMatrixTranspose(lpt);

	result = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }
	
	lightPtr = (LightBuffer*)mappedResource.pData;

	lightPtr->lightView = lvt;
	lightPtr->lightProj = lpt;
	lightPtr->shadowMapSize = shadowMap->GetSize();
	//Spotlight:
	lightPtr->lightPosSpot = spotlight->getPosition();
	lightPtr->lightDirSpot = spotlight->getDirection();
	lightPtr->lightRangeSpot = spotlight->getRange();
	lightPtr->lightConeSpot = spotlight->getCone();
	lightPtr->lightAttSpot = XMFLOAT3(0.3f, 0.03f, 0.0f);
	lightPtr->lightAmbientSpot = spotlight->getAmbientColor();
	lightPtr->lightDiffuseSpot = spotlight->getDiffuseColor();

	std::vector<LightObject*> lights = levelstate->currentLevel->GetLights();

	//End door light:
	lightPtr->lightDiffusePoint1 = lights.at(0)->getDiffuseColor();
	lightPtr->lightPosPoint1 = lights.at(0)->getPosition();
	
	//Menu Pointlight:
	lightPtr->lightDiffusePoint2 = lights.at(1)->getDiffuseColor();
	lightPtr->lightPosPoint2 = lights.at(1)->getPosition();

	//Player Pointlight:
	lightPtr->lightDiffusePoint3 = spotlight->getDiffuseColor();
	lightPtr->lightPosPoint3 = XMFLOAT3(levelstate->currentLevel->getPlayerPostion().x, -1.8f, levelstate->currentLevel->getPlayerPostion().z);

	deviceContext->Unmap(lightBuffer, 0);

	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);


	ID3D11ShaderResourceView* shadowMapSRV = shadowMap->GetShadowSRV();
	deviceContext->PSSetShaderResources(1, 1, &shadowMapSRV);

	return true;
}

void RenderModule::UseDefaultShader()
{
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	deviceContext->IASetInputLayout(layoutPosUvNorm);

	d3d->SetCullingState(2);

	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &sampleStateClamp);
	deviceContext->PSSetSamplers(1, 1, &sampleStateWrap);
	deviceContext->PSSetSamplers(2, 1, &sampleStateComparison);
}

void RenderModule::UseSkeletalShader()
{
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	deviceContext->IASetInputLayout(layoutPosUvNormIdxWei);

	d3d->SetCullingState(3);

	deviceContext->VSSetShader(skeletalVertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &sampleStateClamp);
	deviceContext->PSSetSamplers(1, 1, &sampleStateWrap);
	deviceContext->PSSetSamplers(2, 1, &sampleStateComparison);
}

void RenderModule::UseBlendShader()
{
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	deviceContext->IASetInputLayout(layoutPosUvNorm3PosNorm);

	d3d->SetCullingState(3);

	deviceContext->VSSetShader(blendVertexShader, NULL, 0);
	deviceContext->PSSetShader(pixelShader, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &sampleStateClamp);
	deviceContext->PSSetSamplers(1, 1, &sampleStateWrap);
	deviceContext->PSSetSamplers(2, 1, &sampleStateComparison);
}

bool RenderModule::Render(GameObject* gameObject)
{
	bool result = true;

	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	RenderObject* renderObject = gameObject->GetRenderObject();

	if (renderObject->model->hasSkeleton)
		if (renderObject->model->hasBlendShapes)
		{
			//UseSkeletalBlendShader();
		}
		else
		{
			//throw std::runtime_error("\nDetta objekt: " + gameObject->GetRenderObject()->model->name + "\nbehöver: GameObject* gameObject, float frame");
			UseDefaultShader();
			result = SetDataPerObject(gameObject->GetWorldMatrix(), renderObject, gameObject->GetColorModifier());
		}
	else
		if (renderObject->model->hasBlendShapes)
		{
			throw std::runtime_error("\nDetta objekt: " + gameObject->GetRenderObject()->model->name + "\nbehöver: GameObject* gameObject, float weights[4]");
		}
		else
		{
			UseDefaultShader();
			result = SetDataPerObject(gameObject->GetWorldMatrix(), renderObject, gameObject->GetColorModifier());
		}

	if (!result)
		return false;

	//Now render the prepared buffers with the shader.
	deviceContext->Draw(renderObject->model->vertexBufferSize, 0);

	return result;
}

bool RenderModule::Render(GameObject* gameObject, float frame)
{
	bool result = true;

	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	RenderObject* renderObject = gameObject->GetRenderObject();

	if (renderObject->model->hasSkeleton)
		if (renderObject->model->hasBlendShapes)
		{
			//UseSkeletalBlendShader();
		}
		else
		{
			UseSkeletalShader();
			result = SetDataPerSkeletalObject(gameObject->GetWorldMatrix(), renderObject, false, frame);
		}
	else
		if (renderObject->model->hasBlendShapes)
		{
			throw std::runtime_error("\nDetta objekt: " + gameObject->GetRenderObject()->model->name + "\nbehöver: GameObject* gameObject, float weights[4]");
		}
		else
		{
			UseDefaultShader();
			result = SetDataPerObject(gameObject->GetWorldMatrix(), renderObject, gameObject->GetColorModifier());
		}

	//Set shader parameters, preparing them for render.
	if (!result)
		return false;

	//Now render the prepared buffers with the shader.
	deviceContext->Draw(renderObject->model->vertexBufferSize, 0);

	return result;
}

bool RenderModule::Render(GameObject* gameObject, XMFLOAT4& weights)
{
	bool result = true;

	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	RenderObject* renderObject = gameObject->GetRenderObject();

	if (renderObject->model->hasSkeleton)
	{
		if (renderObject->model->hasBlendShapes)
		{
			//UseSkeletalBlendShader();
			UseBlendShader();
			result = SetDataPerBlendObject(gameObject->GetWorldMatrix(), renderObject, gameObject->GetColorModifier(), weights);
		}
		else
		{
			//throw std::runtime_error("\nDetta objekt: " + gameObject->GetRenderObject()->model->name + "\nbehöver: GameObject* gameObject, float frame");
			UseDefaultShader();
			result = SetDataPerObject(gameObject->GetWorldMatrix(), renderObject, gameObject->GetColorModifier());
		}
	}
	else
	{
		if (renderObject->model->hasBlendShapes)
		{
			UseBlendShader();
			result = SetDataPerBlendObject(gameObject->GetWorldMatrix(), renderObject, gameObject->GetColorModifier(), weights);
		}
		else
		{
			UseDefaultShader();
			result = SetDataPerObject(gameObject->GetWorldMatrix(), renderObject, gameObject->GetColorModifier());
		}
	}

	//Set shader parameters, preparing them for render.
	if (!result)
		return false;

	//Now render the prepared buffers with the shader.
	deviceContext->Draw(renderObject->model->vertexBufferSize, 0);

	return result;
}

void RenderModule::ActivateShadowRendering(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix)
{
	d3d->SetCullingState(2);
	shadowMap->SetDataPerFrame(d3d->GetDeviceContext(), viewMatrix, projectionMatrix);
	shadowMap->ActivateShadowRendering(d3d->GetDeviceContext());
}

bool RenderModule::RenderShadow(GameObject* gameObject)
{
	bool result = true;

	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	RenderObject* renderObject = gameObject->GetRenderObject();

	UINT32 offset = 0;
	UINT32 vertexSize;
	if (renderObject->model->hasBlendShapes)
		vertexSize = sizeof(BlendVertex);
	else
		vertexSize = sizeof(Vertex);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetVertexBuffers(0, 1, &renderObject->model->vertexBuffer, &vertexSize, &offset);

	shadowMap->SetDataPerObject(deviceContext, gameObject->GetWorldMatrix());

	//Now render the prepared buffers with the shader.
	deviceContext->Draw(renderObject->model->vertexBufferSize, 0);
	return result;
}

void RenderModule::BeginScene(float red, float green, float blue, float alpha)
{
	d3d->BeginScene(red, green, blue, alpha);
}
void RenderModule::EndScene()
{
	d3d->EndScene();
}

ID3D11Device* RenderModule::GetDevice()
{
	return d3d->GetDevice();
}
ID3D11DeviceContext* RenderModule::GetDeviceContext()
{
	return d3d->GetDeviceContext();
}