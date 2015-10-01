#include "DirectionalLight.h"


DirectionalLight::DirectionalLight()
{
	spotRange = 15.0f;
	spotCone = 30.0f;
}

DirectionalLight::DirectionalLight(float fov, float aspect, float viewNear, float viewFar, float spotCone, float spotRange)
{
	this->spotCone = spotCone;
	this->spotRange = spotRange;

	projMatrix = XMMatrixPerspectiveFovLH(fov, aspect, viewNear, viewFar);
}

DirectionalLight::~DirectionalLight()
{
}

//Setters
void DirectionalLight::setAmbientColor(float red, float green, float blue, float alpha)
{
	ambientColor = XMFLOAT4(red, green, blue, alpha);
}

void DirectionalLight::setDiffuseColor(float red, float green, float blue, float alpha)
{
	diffuseColor = XMFLOAT4(red, green, blue, alpha);
}


void DirectionalLight::setDirection(float x, float y, float z)
{
	direction = XMFLOAT3(x, y, z);
}

void DirectionalLight::setPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
}

void DirectionalLight::setRange(float range)
{
	spotRange = range;
}

float DirectionalLight::getRange()
{
	return spotRange;
}


void DirectionalLight::setCone(float cone)
{
	spotCone = cone;
}

float DirectionalLight::getCone()
{
	return spotCone;
}

//Getters
XMFLOAT4 DirectionalLight::getAmbientColor()
{
	return ambientColor;
}

XMFLOAT4 DirectionalLight::getDiffuseColor()
{
	return diffuseColor;
}

XMFLOAT3 DirectionalLight::getDirection()
{
	return direction;
}

XMFLOAT3 DirectionalLight::getPosition()
{
	return position;
}

void DirectionalLight::setLookAt(XMFLOAT3 lookAt)
{
	this->lookAt = lookAt;
}

void DirectionalLight::generateViewMatrix()
{
	//setting up a perspective-based view matrix
	XMFLOAT3 up(0.0f, -1.0f, 0.0f);

	DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);
	DirectX::XMVECTOR lookVec = DirectX::XMLoadFloat3(&lookAt);

	// Use the three loaded float3s as vectors for the view matrix
	viewMatrix = XMMatrixLookAtLH(posVec, lookVec, upVec);
}

void DirectionalLight::generateOrthoMatrix(float width, float screenDepth, float screenNear)
{
	orthoMatrix = XMMatrixOrthographicLH(width, width, screenNear, screenDepth);
}

void DirectionalLight::generateProjMatrix(float screenDepth, float screenNear)
{
	float fieldOfView, screenAspect;

	fieldOfView = (float)XM_PI / 2.0f;
	screenAspect = 1.0f;

	projMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
}

void DirectionalLight::getProjMatrix(XMMATRIX& projMatrix)
{
	projMatrix = this->projMatrix;
}

void DirectionalLight::getViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = this->viewMatrix;
}

void DirectionalLight::getOrthoMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = orthoMatrix;
}

void* DirectionalLight::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void DirectionalLight::operator delete(void* p)
{
	_mm_free(p);
}
