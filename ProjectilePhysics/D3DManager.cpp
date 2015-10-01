#include "D3DManager.h"


D3DManager::D3DManager(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	HRESULT result;

	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	//////////////////////////////////////////////////////////// Swap chain, device, device context, rtv //////////////////////////////////////////////////////////
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;

	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen)
		swapChainDesc.Windowed = false;
	else
		swapChainDesc.Windowed = true;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	//Create the swap chain, device, and device context
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);
	if (FAILED(result))
		throw std::runtime_error("D3DManager: Error creating swap chain");

	//Get the pointer to the back buffer
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
		throw std::runtime_error("D3DManager: Could not get swap chain pointer");

	// Create the render target view with the back buffer pointer
	result = device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
	if (FAILED(result))
		throw std::runtime_error("D3DManager: Error creating render target view");

	backBufferPtr->Release();
	backBufferPtr = nullptr;

	//////////////////////////////////////////////////////////////////////// Depth buffer /////////////////////////////////////////////////////////////////////////
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthBuffer);
	if (FAILED(result))
		throw std::runtime_error("D3DManager: Error creating depth buffer");

	///////////////////////////////////////////////////////////////////// Depth stencil states ////////////////////////////////////////////////////////////////////
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//Create depth enable
	result = device->CreateDepthStencilState(&depthStencilDesc, &dsDepthEnable);
	if (FAILED(result))
		throw std::runtime_error("D3DManager: Error creating depth stencil ENABLE");

	depthStencilDesc.DepthEnable = false;

	//Create depth disable
	result = device->CreateDepthStencilState(&depthStencilDesc, &dsDepthDisable);
	if (FAILED(result))
		throw std::runtime_error("D3DManager: Error creating depth stencil DISABLE");



	////////////////////////////////////////////////////////////////////// Depth stencil view /////////////////////////////////////////////////////////////////////
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(depthBuffer, &depthStencilViewDesc, &depthView);
	if (FAILED(result))
		throw std::runtime_error("D3DManager: Error creating depth stencil view");

	///////////////////////////////////////////////////////////////////////// Raster states ///////////////////////////////////////////////////////////////////////
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	//Create raster state BACK
	result = device->CreateRasterizerState(&rasterDesc, &rsBackCulling);
	if (FAILED(result))
		throw std::runtime_error("D3DManager: Error creating raster state BACK");

	//Create raster state FRONT
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	result = device->CreateRasterizerState(&rasterDesc, &rsFrontCulling);
	if (FAILED(result))
		throw std::runtime_error("D3DManager: Error creating raster state FRONT");

	//Create raster state FRONT
	rasterDesc.CullMode = D3D11_CULL_NONE;
	result = device->CreateRasterizerState(&rasterDesc, &rsNoCulling);
	if (FAILED(result))
		throw std::runtime_error("D3DManager: Error creating raster state NONE");

	///////////////////////////////////////////////////////////////////////////// Other ///////////////////////////////////////////////////////////////////////////

	//Setup the viewport for rendering
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	deviceContext->OMSetDepthStencilState(dsDepthEnable, 1);
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthView);
	deviceContext->RSSetState(rsBackCulling);
	deviceContext->RSSetViewports(1, &viewport);
}


D3DManager::~D3DManager()
{
	swapChain->Release();
	device->Release();
	deviceContext->Release();
	renderTargetView->Release();
	depthBuffer->Release();
	depthView->Release();
	dsDepthDisable->Release();
	dsDepthEnable->Release();
	rsBackCulling->Release();
	rsFrontCulling->Release();
	rsNoCulling->Release();
}

void D3DManager::BeginScene(float red, float green, float blue, float alpha)
{
	float color[] = { red, green, blue, alpha };

	deviceContext->OMSetDepthStencilState(dsDepthEnable, 1);
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthView);
	deviceContext->RSSetState(rsBackCulling);
	deviceContext->RSSetViewports(1, &viewport);
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DManager::EndScene()
{
	swapChain->Present(0, 0);
}

ID3D11Device* D3DManager::GetDevice()
{
	return device;
}

ID3D11DeviceContext* D3DManager::GetDeviceContext()
{
	return deviceContext;
}

void D3DManager::SetCullingState(int num) //1 = back, 2 = front, 3 = none
{
	switch (num)
	{
		case 1:
		{
			deviceContext->RSSetState(rsBackCulling);
			break;
		}
		case 2:
		{
			deviceContext->RSSetState(rsFrontCulling);
			break;
		}
		case 3:
		{
			deviceContext->RSSetState(rsNoCulling);
			break;
		}
		default:
			break;
	}
}
void D3DManager::DisableDepth()
{
	deviceContext->OMSetDepthStencilState(dsDepthDisable, 1);
}
void D3DManager::EnableDepth()
{
	deviceContext->OMSetDepthStencilState(dsDepthEnable, 1);
}

void D3DManager::SetBackBufferRenderTarget()
{
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthView);
}

void D3DManager::ResetViewport()
{
	deviceContext->RSSetViewports(1, &viewport);
}
