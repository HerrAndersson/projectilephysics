#include "GameObject.h"

GameObject::GameObject(int id, RenderObject* renderObject, XMFLOAT3 position = XMFLOAT3(0, 0, 0), XMFLOAT3 scale = XMFLOAT3(1, 1, 1), XMFLOAT3 rotation = XMFLOAT3(0, 0, 0))
{
	this->id = id;
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	this->renderObject = renderObject;
	forwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
}

GameObject::GameObject(const GameObject& other)
{
	this->id = other.id;
	this->position = other.position;
	this->rotation = other.rotation;
	this->scale = other.scale;
	this->renderObject = other.renderObject;
	this->forwardVector = other.forwardVector;
}

GameObject::~GameObject()
{
	renderObject = nullptr;
}

XMMATRIX GameObject::GetWorldMatrix()
{

	/*Usually it is scale * rotation * translation.
	However, if you want to rotate an object around a certain point, then it is scale * point_translation * rotation * object_translation.*/
	//R = translate(-P) * rotate(a, r) * translate(P) för att translatera runt en annan punkt p
	//R = translationMatrix(-(p.x, p.y, p.z)) * rotationMatrix((0,0,45) * translationMatrix(p.x, p.y, p.z)


	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
	XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX scalingMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);

	return scalingMatrix * rotationMatrix * translationMatrix;
}
RenderObject* GameObject::GetRenderObject()
{
	return renderObject;
}

void GameObject::SetPosition(XMFLOAT3 pos) 
{
	position = pos;
}

XMFLOAT3 GameObject::GetPosition()
{
	return position;
}

void GameObject::SetRotation(XMFLOAT3 rot)
{
	/*rotation = XMFLOAT3((float)((int)rot.x % 360), (float)((int)rot.y % 360), (float)((int)rot.z % 360));*/

	if (rot.x > 360)
		rot.x -= 360;
	else if (rot.x < 0)
		rot.x = 360 + rot.x;

	if (rot.y > 360)
		rot.y -= 360;
	else if (rot.y < 0)
		rot.y = 360 + rot.y;

	if (rot.z > 360)
		rot.z -= 360;
	else if (rot.z < 0)
		rot.z = 360 + rot.z;

	rotation = rot;
}

XMFLOAT3 GameObject::GetRotation()
{
	return rotation;
}

XMVECTOR GameObject::GetForwardVector()
{
	XMMATRIX rotmat = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
	XMFLOAT4X4 rotationMatrixF;
	XMStoreFloat4x4(&rotationMatrixF, rotmat);
	forwardVector = -XMVectorSet(rotationMatrixF._11, rotationMatrixF._12, rotationMatrixF._13, rotationMatrixF._14);
	return XMVector3Normalize ( forwardVector );
}

ID3D11ShaderResourceView* GameObject::GetDiffuseTexture()
{
	return renderObject->diffuseTexture;
}

void* GameObject::operator new(size_t i)
{
	return _mm_malloc(i, 16);
}

void GameObject::operator delete(void* p)
{
	_mm_free(p);
}