#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <stdexcept>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

class D3DManager
{
private:

	IDXGISwapChain*				swapChain;
	ID3D11Device*			    device;
	ID3D11DeviceContext*		deviceContext;

	ID3D11RenderTargetView*		renderTargetView;
	D3D11_VIEWPORT				viewport;

	ID3D11Texture2D*			depthBuffer;
	ID3D11DepthStencilView*     depthView;

	ID3D11DepthStencilState*	dsDepthDisable; //Used for text and 2d rendering
	ID3D11DepthStencilState*	dsDepthEnable;

	ID3D11RasterizerState*		rsBackCulling;
	ID3D11RasterizerState*		rsFrontCulling; //Used for shadow mapping
	ID3D11RasterizerState*      rsNoCulling;	//Used if we implements our own culling technique

public:

	D3DManager(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen);
	~D3DManager();

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	//1 = back, 2 = front, 3 = none
	void SetCullingState(int num);
	void DisableDepth();
	void EnableDepth();

	void SetBackBufferRenderTarget();
	void ResetViewport();
};

