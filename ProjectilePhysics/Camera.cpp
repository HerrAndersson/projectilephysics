#include "Camera.h"

Camera::Camera(float fovAngleY, int width, int height, float viewNear, float viewFar)
{

	position = XMFLOAT3(0, 0, -20);
	rotation = XMFLOAT3(0, 0, 0);

	this->width = width;
	this->height = height;
	this->aspectRatioWbyH = width / (float)height;
	this->viewNear = viewNear;
	this->viewFar = viewFar;

	//Set where the camera is looking by default.
	camLookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	projectionMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatioWbyH, viewNear, viewFar);
	Update();
}

Camera::~Camera()
{}

void Camera::SetPosition(XMFLOAT3 position)
{
	this->position = position;
	Update();
}

void Camera::SetRotation(XMFLOAT3 rotation)
{
	this->rotation = rotation;
	Update();
}

XMVECTOR Camera::GetLookAt()
{
	return camLookAt;
}

XMFLOAT3 Camera::GetPosition()
{
	return position;
}

XMFLOAT3 Camera::GetRotation()
{
	return rotation;
}

void Camera::Update()
{

	XMMATRIX rotationMatrix;

	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR pos = XMVectorSet(position.x, position.y, position.z, 0);
	XMVECTOR lookAt = XMVectorSet(0, 0, 1, 0);

	rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), 0);

	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	lookAt = pos + lookAt;

	viewMatrix = XMMatrixLookAtLH(pos, lookAt, up);

}

void Camera::SetLookAt(float x, float y, float z)
{
	camLookAt = XMVectorSet(x, y, z, 1);
	Update();
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

