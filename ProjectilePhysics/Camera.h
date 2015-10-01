#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
private:

	float positionX, positionY, positionZ;
	float rotationX, rotationY, rotationZ;
	float aspectRatioWbyH;
	int width, height;
	float viewNear, viewFar;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	XMVECTOR camUp;
	XMVECTOR camLookAt;

public:

	Camera(float fovAngleY, int width, int height, float viewNear, float viewFar);
	~Camera();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetLookAt(float x, float y, float z);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMVECTOR GetLookAt();
	XMVECTOR GetCamUp();
	const float GetAspectRatio();

	void Update();
	void GetViewMatrix(XMMATRIX& viewMatrix);
	void GetProjectionMatrix(XMMATRIX& projectionMatrix);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);
};
