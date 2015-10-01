#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class DirectionalLight
{

private:

	XMMATRIX viewMatrix;
	XMMATRIX orthoMatrix;
	XMMATRIX projMatrix;

	XMFLOAT4 ambientColor;
	XMFLOAT4 diffuseColor;
	XMFLOAT3 position;
	XMFLOAT3 direction;
	XMFLOAT3 lookAt;

	float spotRange;
	float spotCone;

public:

	DirectionalLight();
	DirectionalLight(float fov, float aspect, float viewNear, float viewFar, float spotCone, float spotRange);
	~DirectionalLight();

	void setAmbientColor(float r, float g, float b, float a);
	void setDiffuseColor(float r, float g, float b, float a);
	void setDirection(float x, float y, float z);
	void setPosition(float x, float y, float z);
	void setLookAt(XMFLOAT3 lookAt);

	void setRange(float range);
	void setCone(float cone);
	float getRange();
	float getCone();

	XMFLOAT4 getAmbientColor();
	XMFLOAT4 getDiffuseColor();
	XMFLOAT3 getDirection();
	XMFLOAT3 getPosition();

	//Different types of view matrices; a regular perspective and an orthographic one.
	//These could in a later case be applied to create shadow maps.

	void generateViewMatrix();
	void getViewMatrix(XMMATRIX& viewMatrix);

	void generateProjMatrix(float screenDepth, float screenNear);
	void getProjMatrix(XMMATRIX& projMatrix);

	void generateOrthoMatrix(float width, float screenDepth, float screenNear);
	void getOrthoMatrix(XMMATRIX& orthoMatrix);

	//Overloading these guarantees 16B alignment of XMMATRIX
	void* operator new(size_t i);
	void operator delete(void* p);

};
