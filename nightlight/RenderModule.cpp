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
	matrixBufferPerFrame = NULL;
	lightBuffer = NULL;
	this->hwnd = hwnd;

	d3d = new D3DManager(hwnd, screenWidth, screenHeight, fullscreen);
	shadowMap = new ShadowMap(d3d->GetDevice(), shadowMapSize, L"Assets/Shaders/ShadowVS.hlsl");

	bool result;

	InitializeSamplers();
	InitializeConstantBuffers();

	result = InitializeShader(L"Assets/Shaders/vertexShader.hlsl", L"Assets/Shaders/pixelShader.hlsl");
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

	if (FAILED(result))
		throw std::runtime_error("RenderModule: Failed to create MatrixBufferPerBlendObject");

	return true;
}

RenderModule::~RenderModule()
{
	delete d3d;
	delete shadowMap;

	layoutPosUvNorm->Release();

	matrixBufferPerObject->Release();
	matrixBufferPerFrame->Release();

	pixelShader->Release();
	vertexShader->Release();

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

bool RenderModule::SetDataPerObject(XMMATRIX& worldMatrix, RenderObject* renderObject, XMFLOAT3 colorModifier)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();


	UINT32 vertexSize;
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

bool RenderModule::SetDataPerFrame(XMMATRIX& viewMatrix, XMMATRIX& projectionMatrix, XMFLOAT3& camPos)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferPerFrame* dataPtr;
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

bool RenderModule::Render(GameObject* gameObject)
{
	bool result = true;

	ID3D11DeviceContext* deviceContext = d3d->GetDeviceContext();
	RenderObject* renderObject = gameObject->GetRenderObject();

	UseDefaultShader();
	result = SetDataPerObject(gameObject->GetWorldMatrix(), renderObject, gameObject->GetColorModifier());
		

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