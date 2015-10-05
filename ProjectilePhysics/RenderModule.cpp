#include "RenderModule.h"
#include <fstream>
#include <string>
using namespace std;


RenderModule::RenderModule(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen, int shadowMapSize)
{
	vsDefault = nullptr;
	psDefault = nullptr;
	psTerrain = nullptr;
	sampleStateWrap = nullptr;
	sampleStateClamp = nullptr;
	matrixBufferPerObject = nullptr;
	matrixBufferPerFrame = nullptr;
	lightBuffer = nullptr;
	this->hwnd = hwnd;

	d3d = new D3DManager(hwnd, screenWidth, screenHeight, fullscreen);
	shadowMap = new ShadowMap(d3d->GetDevice(), shadowMapSize, L"Assets/Shaders/ShadowVS.hlsl");

	bool result;

	InitializeSamplers();
	InitializeConstantBuffers();

	result = InitializeShaders(L"Assets/Shaders/DefaultVS.hlsl", L"Assets/Shaders/DefaultPS.hlsl", L"Assets/Shaders/TerrainPS.hlsl");
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
		throw std::runtime_error("RenderModule(InitializeSamplers): samplerStateWrap initialization failed.");

	//Create a CLAMP texture sampler state description.
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	result = device->CreateSamplerState(&samplerDesc, &sampleStateClamp);
	if (FAILED(result))
		throw std::runtime_error("RenderModule(InitializeSamplers): samplerStateClamp initialization failed.");

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
		throw std::runtime_error("RenderModule(InitializeSamplers): samplerStateComparison initialization failed.");

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
		throw std::runtime_error("RenderModule(InitializeConstantBuffers): Failed to create MatrixBufferPerObject");

	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerFrame);
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBufferPerFrame);

	if (FAILED(result))
		throw std::runtime_error("RenderModule(InitializeConstantBuffers): Failed to create MatrixBufferPerFrame");

	matrixBufferDesc.ByteWidth = sizeof(LightBuffer);
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &lightBuffer);

	if (FAILED(result))
		throw std::runtime_error("RenderModule: Failed to create LightBuffer");


	return true;
}

RenderModule::~RenderModule()
{
	delete d3d;
	delete shadowMap;

	layoutPosUvNorm->Release();

	matrixBufferPerObject->Release();
	matrixBufferPerFrame->Release();

	psDefault->Release();
	psTerrain->Release();
	vsDefault->Release();

	sampleStateClamp->Release();
	sampleStateWrap->Release();
	sampleStateComparison->Release();
}

bool RenderModule::InitializeShaders(WCHAR* vsDefaultName, WCHAR* psDefaultName, WCHAR* psTerrainName)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;

	ID3D11Device* device = d3d->GetDevice();

	/////////////////////////////////////////////////////////////////////////// Shaders ///////////////////////////////////////////////////////////////////////////
	if (!vsDefault)
	{
		result = D3DCompileFromFile(vsDefaultName, NULL, NULL, "main", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
				throw runtime_error("RenderModule(InitializeShader): " + string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			else
				throw std::runtime_error("RenderModule(InitializeShader): Vertexshader not found");;

			return false;
		}

		result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vsDefault);
		if (FAILED(result))
			return false;
	}

	if (!psDefault)
	{
		result = D3DCompileFromFile(psDefaultName, NULL, NULL, "main", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
				throw runtime_error("RenderModule(InitializeShader) :" + string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			else
				throw std::runtime_error("RenderModule(InitializeShader): Pixelshader not found");

			return false;
		}

		result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &psDefault);
		if (FAILED(result))
			return false;

	}

	if (!psTerrain)
	{
		result = D3DCompileFromFile(psTerrainName, NULL, NULL, "main", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
				throw runtime_error("RenderModule(InitializeShader) :" + string(static_cast<const char*>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
			else
				throw std::runtime_error("RenderModule(InitializeShader): Terrainshader not found");

			return false;
		}

		result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &psTerrain);
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

bool RenderModule::SetDataPerObject(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 colorModifier)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	UINT32 vertexSize;
	vertexSize = sizeof(Vertex);

	UINT32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &renderObject->model->vertexBuffer, &vertexSize, &offset);
	deviceContext->PSSetShaderResources(1, 1, &renderObject->diffuseTexture);
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

bool RenderModule::SetDataPerFrame(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, XMFLOAT3 camPos, XMFLOAT3 lightPos, XMMATRIX& lightView, XMMATRIX& lightProjection)
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
	lvt = XMMatrixTranspose(lightView);
	lpt = XMMatrixTranspose(lightProjection);

	result = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	lightPtr = (LightBuffer*)mappedResource.pData;

	lightPtr->lightView = lvt;
	lightPtr->lightProjection = lpt;
	lightPtr->lightPos = lightPos;
	lightPtr->shadowMapSize = shadowMap->GetSize();

	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	ID3D11ShaderResourceView* shadowMapSRV = shadowMap->GetShadowSRV();
	deviceContext->PSSetShaderResources(0, 1, &shadowMapSRV);


	return true;
}

void RenderModule::UseDefaultShader()
{
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	deviceContext->IASetInputLayout(layoutPosUvNorm);

	d3d->SetCullingState(CullingState::BACK);

	deviceContext->VSSetShader(vsDefault, NULL, 0);
	deviceContext->PSSetShader(psDefault, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &sampleStateClamp);
	deviceContext->PSSetSamplers(1, 1, &sampleStateWrap);
	deviceContext->PSSetSamplers(2, 1, &sampleStateComparison);
}

void RenderModule::UseTerrainShader()
{
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	deviceContext->IASetInputLayout(layoutPosUvNorm);

	d3d->SetCullingState(CullingState::BACK);

	deviceContext->VSSetShader(vsDefault, NULL, 0);
	deviceContext->PSSetShader(psTerrain, NULL, 0);

	deviceContext->PSSetSamplers(0, 1, &sampleStateClamp);
	deviceContext->PSSetSamplers(1, 1, &sampleStateWrap);
	deviceContext->PSSetSamplers(2, 1, &sampleStateComparison);
}

bool RenderModule::Render(GameObject* gameObject)
{
	bool result = true;

	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	RenderObject* renderObject = gameObject->GetRenderObject();

	result = SetDataPerObject(gameObject->GetWorldMatrix(), renderObject, gameObject->GetColorModifier());
		
	if (!result)
		return false;

	//Now render the prepared buffers with the shader.
	deviceContext->Draw(renderObject->model->vertexBufferSize, 0);

	return result;
}

void RenderModule::ActivateShadowRendering(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix)
{
	d3d->SetCullingState(CullingState::FRONT);
	shadowMap->SetDataPerFrame(d3d->GetDeviceContext(), viewMatrix, projectionMatrix);
	shadowMap->ActivateShadowRendering(d3d->GetDeviceContext());
}

void RenderModule::SetCullingState(CullingState state)
{
	d3d->SetCullingState(state);
}

bool RenderModule::RenderShadow(GameObject* gameObject)
{
	bool result = true;

	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	RenderObject* renderObject = gameObject->GetRenderObject();

	UINT32 offset = 0;
	UINT32 vertexSize;

	vertexSize = sizeof(Vertex);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetVertexBuffers(0, 1, &renderObject->model->vertexBuffer, &vertexSize, &offset);

	shadowMap->SetDataPerObject(deviceContext, gameObject->GetWorldMatrix());

	//Now render the prepared buffers with the shader.
	deviceContext->Draw(renderObject->model->vertexBufferSize, 0);
	return result;
}

bool RenderModule::RenderShadow(Terrain* terrain)
{
	bool result = true;

	XMMATRIX worldMatrix = XMMatrixIdentity();
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	UINT32 vertexSize = sizeof(Vertex);;
	UINT32 offset = 0;

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	terrain->GetBuffers(vertexBuffer, indexBuffer);

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	shadowMap->SetDataPerObject(deviceContext, worldMatrix);

	//Now render the prepared buffers with the shader.
	d3d->GetDeviceContext()->DrawIndexed(terrain->GetVertexCount(), 0, 0);

	return true;
}

bool RenderModule::SetTerrainData(XMMATRIX& worldMatrix, XMFLOAT3 colorModifier, Terrain* terrain)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();

	UINT32 vertexSize = sizeof(Vertex);;
	UINT32 offset = 0;

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	terrain->GetBuffers(vertexBuffer, indexBuffer);
	ID3D11ShaderResourceView** textures = terrain->GetTextures();

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->PSSetShaderResources(1, 4, textures);

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

bool RenderModule::RenderTerrain(Terrain* terrain)
{
	d3d->GetDeviceContext()->DrawIndexed(terrain->GetVertexCount(), 0, 0);
	return true;
}

void RenderModule::DrawString(wstring text, FLOAT fontSize, FLOAT posX, FLOAT posY, UINT32 color)
{
	d3d->DrawString(text, fontSize, posX, posY, color);

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