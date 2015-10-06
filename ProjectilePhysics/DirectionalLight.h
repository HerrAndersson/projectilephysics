#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class DirectionalLight
{

private:

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	XMFLOAT3 position;
	XMFLOAT3 lookAt;
	XMFLOAT3 rotation;

	float fov, aspect;
	float viewNear, viewFar;

	void GenerateProjectionMatrix();
	void GenerateViewMatrix();

public:

	DirectionalLight(float fov, float aspect, float viewNear, float viewFar);
	~DirectionalLight();

	void SetPosition(XMFLOAT3 position);
	void SetLookAt(XMFLOAT3 lookAt);

	XMFLOAT3 GetPosition();
	void GetViewMatrix(XMMATRIX& viewMatrix);
	void GetProjectionMatrix(XMMATRIX& projectionMatrix);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

};
