#include "CameraObject.h"

CameraObject::CameraObject(float fovAngleY, int width, int height, float viewNear, float viewFar)
{
	positionX = 0.0f;
	positionY = 20.0f;
	positionZ = 0.0f;

	rotationX = 0.0f;
	rotationY = 0.0f;
	rotationZ = 0.0f;

	this->width = width;
	this->height = height;
	this->aspectRatioWbyH = width / (float)height;
	this->viewNear = viewNear;
	this->viewFar = viewFar;

	//Set where the camera is looking by default.
	camLookAt = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);

	projectionMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatioWbyH, viewNear, viewFar);
	UpdateCamera();
}

CameraObject::~CameraObject()
{}

void CameraObject::SetPosition(float x, float y, float z)
{
	positionX = x;
	positionY = y;
	positionZ = z;
	UpdateCamera();
}

void CameraObject::SetRotation(float x, float y, float z)
{
	rotationX = x;
	rotationY = y; 
	rotationZ = z;
	UpdateCamera();
}

XMVECTOR CameraObject::GetLookAt()
{
	return camLookAt;
}

XMFLOAT3 CameraObject::GetPosition()
{
	return XMFLOAT3(positionX, positionY, positionZ);
}


XMFLOAT3 CameraObject::GetRotation()
{
	return XMFLOAT3(rotationX, rotationY, rotationZ);
}

void CameraObject::UpdateCamera()
{
	XMVECTOR up, position;
	XMMATRIX rotationMatrix;
	XMMATRIX translationMatrix;

	position = XMVectorSet(positionX, positionY, positionZ, 1.0f);

	rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotationX), XMConvertToRadians(rotationY), XMConvertToRadians(rotationZ));
	
	up = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	up = XMVector3TransformCoord(up, rotationMatrix);

	camUp = up;

	//Create the view matrix from the updated vectors.
	viewMatrix = XMMatrixLookAtLH(position, camLookAt, up);
}

void CameraObject::SetLookAt(float x, float y, float z)
{
	camLookAt = XMVectorSet(x, y, z, 1);
}

XMVECTOR CameraObject::GetCamUp()
{
	return camUp;
}

void CameraObject::GetProjectionMatrix(XMMATRIX& projectionMatrixIn)
{
	projectionMatrixIn = projectionMatrix;
}

void CameraObject::GetViewMatrix(XMMATRIX& viewMatrixIn)
{
	viewMatrixIn = viewMatrix;
}

const float CameraObject::GetAspectRatio()
{
	return aspectRatioWbyH;
}

void* CameraObject::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void CameraObject::operator delete(void* p)
{
	_mm_free(p);
}

