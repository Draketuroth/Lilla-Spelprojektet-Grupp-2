#include "CharacterBase.h"
#include <sstream>

CharacterBase::CharacterBase()
{
	this->health = 0;
	this->movementSpeed = 20;
	this->unitID = 0;
	this->alive = true;
	this->timer.initialize();
	this->position = { 0, 0, 0 };
	this->tPlayerTranslation = XMMatrixIdentity();
}

CharacterBase::CharacterBase(const bool alive, const int health, const float movemenstpeed, const int unitID, const XMFLOAT3 position, const XMMATRIX tPlayerTranslation)
{
	this->health = health;
	this->movementSpeed = movemenstpeed;
	this->unitID = unitID;
	this->alive = alive;
	this->position = position;
	this->tPlayerTranslation = tPlayerTranslation;
	this->timer.initialize();
}

CharacterBase::~CharacterBase()
{
	//Safe release
}

void CharacterBase::releaseAll() {

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
}

int CharacterBase::getHealth()const
{
	return this->health;
}
void CharacterBase::setHealth(const int newHealth)
{
	this->health = newHealth;
}

float CharacterBase::getMovementSpeed()const
{
	return this->movementSpeed;
}
void CharacterBase::setMovementSpeed(const float newMSpeed)
{
	this->movementSpeed = newMSpeed;
}

int CharacterBase::getUnitID()const
{
	return this->unitID;
}
void CharacterBase::setUnitID(const int newUnitID)
{
	this->unitID = newUnitID;
}

bool CharacterBase::getAlive()const
{
	return this->alive;
}
void CharacterBase::setAlive(const bool newAlive)
{
	this->alive = newAlive;
}

XMFLOAT3 CharacterBase::getPos()const
{
	return this->position;
}
void CharacterBase::setPos(const XMFLOAT3 newPos)
{
	this->position = newPos;
}
//-------------Create Buffer and Draw -----------------------
bool CharacterBase::createBuffers(ID3D11Device* &graphicDevice)
{
	//----------CUBE-------------------------
	HRESULT hr;

	TriangleVertex cubeVertices[24] =
	{
		//Front face
		-0.1f,  0.1f, -0.1f, 0.0f, 0.0f,
		 0.1f,  0.1f, -0.1f, 1.0f, 0.0f,
		-0.1f, -0.1f, -0.1f, 0.0f, 1.0f,
		 0.1,  -0.1f, -0.1f, 1.0f, 1.0f,

		// Back face
	 	 0.1f,  0.1f, 0.1f, 0.0f, 0.0f,
		-0.1f,  0.1f, 0.1f, 1.0f, 0.0f,
		 0.1f, -0.1f, 0.1f, 0.0f, 1.0f,
		-0.1,  -0.1f, 0.1f, 1.0f, 1.0f,

		// Left face
		-0.1f,  0.1f,  0.1f, 0.0f, 0.0f,
		-0.1f,  0.1f, -0.1f, 1.0f, 0.0f,
		-0.1f, -0.1f,  0.1f, 0.0f, 1.0f,
		-0.1f, -0.1f, -0.1f, 1.0f, 1.0f,

		// Right face
		0.1f,  0.1f, -0.1f, 0.0f, 0.0f,
		0.1f,  0.1f,  0.1f, 1.0f, 0.0f,
		0.1f, -0.1f, -0.1f, 0.0f, 1.0f,
		0.1f, -0.1f,  0.1f, 1.0f, 1.0f,

		// Top face
		-0.1f, 0.1f,  0.1f, 0.0f, 0.0f,
		 0.1f, 0.1f,  0.1f, 1.0f, 0.0f,
		-0.1f, 0.1f, -0.1f, 0.0f, 1.0f,
		 0.1f, 0.1f, -0.1f, 1.0f, 1.0f,

		// Bottom face
		 0.1f, -0.1f,  0.1f, 0.0f, 0.0f,
		-0.1f, -0.1f,  0.1f, 1.0f, 0.0f,
		 0.1f, -0.1f, -0.1f, 0.0f, 1.0f,
		-0.1f, -0.1f, -0.1f, 1.0f, 1.0f
	};

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(cubeVertices);

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = cubeVertices;
	hr = graphicDevice->CreateBuffer(&bufferDesc, &data, &vertexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	// Create Indices
	unsigned int indices[36] = {

		// Front face
		0,1,2,
		2,1,3,

		// Back face

		4,5,6,
		6,5,7,

		// Left face

		8,9,10,
		10,9,11,

		// Right face

		12,13,14,
		14,13,15,

		// Top face

		16,17,18,
		18,17,19,

		// Bottom face

		20,21,22,
		22,21,23 };

	// Create the buffer description
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(indices);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Set the subresource data

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Create the buffer

	hr = graphicDevice->CreateBuffer(&bufferDesc, &initData, &indexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}
void CharacterBase::draw(ID3D11DeviceContext* &graphicDeviceContext) {

	ID3D11ShaderResourceView* nullSRV = nullptr;

	graphicDeviceContext->PSSetShaderResources(0, 1, &nullSRV);

	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;

	graphicDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
	graphicDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	graphicDeviceContext->DrawIndexed(36, 0, 0);	
}
//-----------------------------------------------------------
//-------------Move and WorldMatrix -------------------------
void CharacterBase::move(XMFLOAT3 direction)
{
	XMFLOAT3 newPosition(position.x + direction.x, position.y + direction.y, position.z + direction.z);
	setPos(newPosition);
}

void CharacterBase::updateWorldMatrix(XMFLOAT3 newPos)
{
	tPlayerTranslation = XMMatrixTranspose(XMMatrixTranslation(newPos.x, newPos.y, newPos.z));
	//tPlayerTranslation = XMMatrixTranslation(direction.x, direction.y, direction.z);
}
void CharacterBase::resetWorldMatrix()
{
	this->tPlayerTranslation = XMMatrixIdentity();
}

//-----------------------------------------------------------

string CharacterBase::toString()
{
	stringstream toString;

	toString << "Unit ID: " << this->unitID << endl 
		<< "Health: " << this->health << endl 
		<< "Movement Speed: " << this->movementSpeed << endl 
		<< "Alive: " << this->alive << endl;

	return toString.str();
}
