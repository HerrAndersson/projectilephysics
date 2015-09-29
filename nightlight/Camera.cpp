#include "Camera.h"

Camera::Camera(float fovAngleY, int width, int height, float viewNear, float viewFar)
{
	positionX = 0.0f;
	positionY = 0.0f;
	positionZ = -20.0f;

	rotationX = 0.0f;
	rotationY = 0.0f;
	rotationZ = 0.0f;

	this->width = width;
	this->height = height;
	this->aspectRatioWbyH = width / (float)height;
	this->viewNear = viewNear;
	this->viewFar = viewFar;

	//Set where the camera is looking by default.
	camLookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	projectionMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatioWbyH, viewNear, viewFar);
	UpdateCamera();
}

Camera::~Camera()
{}

void Camera::SetPosition(float x, float y, float z)
{
	positionX = x;
	positionY = y;
	positionZ = z;
	UpdateCamera();
}

void Camera::SetRotation(float x, float y, float z)
{
	rotationX = x;
	rotationY = y; 
	rotationZ = z;
	UpdateCamera();
}

XMVECTOR Camera::GetLookAt()
{
	return camLookAt;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(positionX, positionY, positionZ);
}


XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(rotationX, rotationY, rotationZ);
}

void Camera::UpdateCamera()
{
	XMMATRIX rotationMatrix;

	XMVECTOR position = XMVectorSet(positionX, positionY, positionZ, 1.0f);

	rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotationX), XMConvertToRadians(rotationY), XMConvertToRadians(rotationZ));
	
	camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	camUp = XMVector3TransformCoord(camUp, rotationMatrix);

	//Create the view matrix from the updated vectors.
	viewMatrix = XMMatrixLookAtLH(position, camLookAt, camUp);
}

void Camera::SetLookAt(float x, float y, float z)
{
	camLookAt = XMVectorSet(x, y, z, 1);
}

XMVECTOR Camera::GetCamUp()
{
	return camUp;
}

void Camera::GetProjectionMatrix(XMMATRIX& projectionMatrixIn)
{
	projectionMatrixIn = projectionMatrix;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrixIn)
{
	viewMatrixIn = viewMatrix;
}

const float Camera::GetAspectRatio()
{
	return aspectRatioWbyH;
}

void* Camera::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void Camera::operator delete(void* p)
{
	_mm_free(p);
}

