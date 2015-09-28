#include "LightObject.h"


LightObject::LightObject()
{
	spotRange = 15.0f;
	spotCone = 30.0f;
}

LightObject::LightObject(float fov, float aspect, float viewNear, float viewFar, float spotCone, float spotRange)
{
	this->spotCone = spotCone;
	this->spotRange = spotRange;

	projMatrix = XMMatrixPerspectiveFovLH(fov, aspect, viewNear, viewFar);
}

LightObject::~LightObject()
{
}

//Setters
void LightObject::setAmbientColor(float red, float green, float blue, float alpha)
{
	ambientColor = XMFLOAT4(red, green, blue, alpha);
}

void LightObject::setDiffuseColor(float red, float green, float blue, float alpha)
{
	diffuseColor = XMFLOAT4(red, green, blue, alpha);
}


void LightObject::setDirection(float x, float y, float z)
{
	direction = XMFLOAT3(x, y, z);
}

void LightObject::setPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
}

void LightObject::setRange(float range)
{
	spotRange = range;
}

float LightObject::getRange()
{
	return spotRange;
}


void LightObject::setCone(float cone)
{
	spotCone = cone;
}

float LightObject::getCone()
{
	return spotCone;
}

//Getters
XMFLOAT4 LightObject::getAmbientColor()
{
	return ambientColor;
}

XMFLOAT4 LightObject::getDiffuseColor()
{
	return diffuseColor;
}

XMFLOAT3 LightObject::getDirection()
{
	return direction;
}

XMFLOAT3 LightObject::getPosition()
{
	return position;
}

void LightObject::setLookAt(XMFLOAT3 lookAt)
{
	this->lookAt = lookAt;
}

void LightObject::generateViewMatrix()
{
	//setting up a perspective-based view matrix
	XMFLOAT3 up(0.0f, -1.0f, 0.0f);

	DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);
	DirectX::XMVECTOR lookVec = DirectX::XMLoadFloat3(&lookAt);

	// Use the three loaded float3s as vectors for the view matrix
	viewMatrix = XMMatrixLookAtLH(posVec, lookVec, upVec);
}

void LightObject::generateOrthoMatrix(float width, float screenDepth, float screenNear)
{
	orthoMatrix = XMMatrixOrthographicLH(width, width, screenNear, screenDepth);
}

void LightObject::generateProjMatrix(float screenDepth, float screenNear)
{
	float fieldOfView, screenAspect;

	fieldOfView = (float)XM_PI / 2.0f;
	screenAspect = 1.0f;

	projMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
}

void LightObject::getProjMatrix(XMMATRIX& projMatrix)
{
	projMatrix = this->projMatrix;
}

void LightObject::getViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = this->viewMatrix;
}

void LightObject::getOrthoMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = orthoMatrix;
}

void* LightObject::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void LightObject::operator delete(void* p)
{
	_mm_free(p);
}
