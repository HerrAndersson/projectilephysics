#include "ShadowMap.h"

using std::runtime_error;
using std::string;

ShadowMap::ShadowMap(ID3D11Device* device, int dimensions, LPCWSTR vsFilename)
{
	this->dimensions = dimensions;

	HRESULT hr;
	ID3DBlob* errorMessage = nullptr;
	ID3DBlob* pVS = nullptr;

	///////////////////////////////////////////////////////// T2D, DSV, SRV /////////////////////////////////////////////////////////
	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	//shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	shadowMapDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 1;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	shadowMapDesc.Height = (UINT)dimensions;
	shadowMapDesc.Width = (UINT)dimensions;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	//depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hr = device->CreateTexture2D(&shadowMapDesc, nullptr, &shadowMap);
	if (FAILED(hr))
		throw runtime_error("ShadowMap: Could not create Shadow map Texture2D");

	hr = device->CreateDepthStencilView(shadowMap, &depthStencilViewDesc, &shadowDepthView);
	if (FAILED(hr))
		throw runtime_error("ShadowMap: Could not create Shadow map DSV");
	
	hr = device->CreateShaderResourceView(shadowMap, &shaderResourceViewDesc, &shadowResourceView);
	if (FAILED(hr))
		throw runtime_error("ShadowMap: Could not create Shadow map SRV");

	///////////////////////////////////////////////////////// Vertex shader /////////////////////////////////////////////////////////
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = D3DCompileFromFile(vsFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_4_0", NULL, NULL, &pVS, &errorMessage);

	if (FAILED(hr))
	{
		if (errorMessage)
			throw runtime_error("ShadowMap: " + string(static_cast<const char *>(errorMessage->GetBufferPointer()), errorMessage->GetBufferSize()));
		else
			throw runtime_error("ShadowMap: Shadow Vertex file missing");
	}

	device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &shadowVS);
	hr = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &shadowInputLayout);
	pVS->Release();

	/////////////////////////////////////////////////////////// Buffers /////////////////////////////////////////////////////////////
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerObject);
	hr = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBufferPerObject);

	if (FAILED(hr))
		throw std::runtime_error("ShadowMap: Failed to create matrixBufferPerObject");

	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferPerFrame);
	hr = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBufferPerFrame);

	if (FAILED(hr))
		throw std::runtime_error("ShadowMap: Failed to create matrixBufferPerFrame");

	///////////////////////////////////////////////////////////// Other /////////////////////////////////////////////////////////////
	ZeroMemory(&shadowViewport, sizeof(D3D11_VIEWPORT));
	shadowViewport.Height = (FLOAT)dimensions;
	shadowViewport.Width = (FLOAT)dimensions;
	shadowViewport.MinDepth = 0.f;
	shadowViewport.MaxDepth = 1.f;

	D3D11_DEPTH_STENCIL_DESC shadowDepthStencilDesc;
	ZeroMemory(&shadowDepthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	shadowDepthStencilDesc.DepthEnable = true;
	shadowDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	shadowDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	shadowDepthStencilDesc.StencilEnable = false;

	hr = device->CreateDepthStencilState(&shadowDepthStencilDesc, &shadowDepthState);
	if (FAILED(hr))
		throw runtime_error("ShadowMap: Shadow Depth stencil error");
}

ShadowMap::~ShadowMap()
{
	shadowMap->Release();
	shadowDepthView->Release();
	shadowResourceView->Release();
}

void ShadowMap::ActivateShadowRendering(ID3D11DeviceContext* deviceContext)
{
	ID3D11ShaderResourceView* nullSrv = nullptr;
	deviceContext->PSSetShaderResources(0, 1, &nullSrv);

	deviceContext->ClearDepthStencilView(shadowDepthView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->OMSetRenderTargets(0, nullptr, shadowDepthView);
	deviceContext->OMSetDepthStencilState(shadowDepthState, 1);
	deviceContext->IASetInputLayout(shadowInputLayout);
	deviceContext->VSSetShader(shadowVS, nullptr, 0);
	deviceContext->RSSetViewports(1, &shadowViewport);

	//Unbind PS since it's not used
	ID3D11PixelShader* nullPS = nullptr;
	deviceContext->PSSetShader(nullPS, nullptr, 0);
}

void ShadowMap::SetDataPerObject(ID3D11DeviceContext* deviceContext, XMMATRIX& modelWorld)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	XMMATRIX twvp = XMMatrixTranspose(modelWorld);

	hr = deviceContext->Map(matrixBufferPerObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	MatrixBufferPerObject* matrixDataBuffer = (MatrixBufferPerObject*)mappedResource.pData;

	matrixDataBuffer->modelWorld = twvp;

	deviceContext->Unmap(matrixBufferPerObject, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &matrixBufferPerObject);
}

void ShadowMap::SetDataPerFrame(ID3D11DeviceContext* deviceContext, XMMATRIX& lightView, XMMATRIX& lightProj)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	XMMATRIX lvt = XMMatrixTranspose(lightView);
	XMMATRIX lpt = XMMatrixTranspose(lightProj);

	hr = deviceContext->Map(matrixBufferPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	MatrixBufferPerFrame* matrixDataBuffer = (MatrixBufferPerFrame*)mappedResource.pData;

	matrixDataBuffer->lightView = lvt;
	matrixDataBuffer->lightProj = lpt;

	deviceContext->Unmap(matrixBufferPerFrame, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &matrixBufferPerFrame);
}

ID3D11ShaderResourceView* ShadowMap::GetShadowSRV()
{
	return shadowResourceView;
}

int ShadowMap::GetSize()
{
	return dimensions;
}