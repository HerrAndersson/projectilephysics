#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(float fov, float aspect, float viewNear, float viewFar)
{
	this->fov = fov;
	this->aspect = aspect;
	this->viewNear = viewNear;
	this->viewFar = viewFar;
	
	GenerateProjectionMatrix();
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::SetPosition(XMFLOAT3 position)
{
	this->position = position;
	GenerateViewMatrix();
}


XMFLOAT3 DirectionalLight::GetPosition()
{
	return position;
}

void DirectionalLight::SetLookAt(XMFLOAT3 lookAt)
{
	this->lookAt = lookAt;
	GenerateViewMatrix();
}

void DirectionalLight::GenerateViewMatrix()
{

	DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR upVec = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	DirectX::XMVECTOR lookVec = DirectX::XMLoadFloat3(&lookAt);
	lookVec = XMVectorSetW(lookVec, 1.0f);

	viewMatrix = XMMatrixLookAtLH(posVec, lookVec, upVec);
}

void DirectionalLight::GenerateProjectionMatrix()
{
	projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspect, viewNear, viewFar);
}

void DirectionalLight::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = this->projectionMatrix;
}

void DirectionalLight::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = this->viewMatrix;
}

void* DirectionalLight::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void DirectionalLight::operator delete(void* p)
{
	_mm_free(p);
}
