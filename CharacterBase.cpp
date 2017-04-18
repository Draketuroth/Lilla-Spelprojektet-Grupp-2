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
bool CharacterBase::createBuffers(ID3D11Device* &graphicDevice, vector<Vertex_Bone>fbxVector, FbxImport &fbxImporter, VS_SKINNED_DATA &skinData)
{
	HRESULT hr;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// BONE BUFFER DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_BUFFER_DESC boneBufferDesc;

	memset(&boneBufferDesc, 0, sizeof(boneBufferDesc));

	boneBufferDesc.ByteWidth = sizeof(VS_SKINNED_DATA);
	boneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	boneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	boneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	boneBufferDesc.MiscFlags = 0;
	boneBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA boneData;
	boneData.pSysMem = &skinData;
	boneData.SysMemPitch = 0;
	boneData.SysMemSlicePitch = 0;

	hr = graphicDevice->CreateBuffer(&boneBufferDesc, &boneData, &fbxImporter.gBoneBuffer);

	if (FAILED(hr)) {

		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------------------//

	//----------------------------------------------------------------------------------------------------------------------------------//
	// VERTEX BUFFER DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_BUFFER_DESC bufferDesc = {};

	memset(&bufferDesc, 0, sizeof(bufferDesc));

	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = fbxVector.size() * sizeof(Vertex_Bone);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &fbxVector[0];
	hr = graphicDevice->CreateBuffer(&bufferDesc, &data, &fbxImporter.gBoneVertexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

void CharacterBase::CreateBoundingBox(float mass, XMFLOAT3 spawnPos, BulletComponents &bulletPhysicsHandler) {

	//----------------------------------------------------------------------//
	// CREATE THE RIGID BODY
	//----------------------------------------------------------------------//

	// Platform Rigid Body only uses an identity matrix as its world matrix. Might have to be changed later
	XMMATRIX translation = XMMatrixTranslation(spawnPos.x, spawnPos.y, spawnPos.z);
	XMFLOAT4X4 t;
	XMStoreFloat4x4(&t, translation);

	btTransform transform;
	transform.setFromOpenGLMatrix((float*)&t);

	// Define the kind of shape we want and construct rigid body information
	btBoxShape* boxShape = new btBoxShape(btVector3(1.3, 1.3, 1.3));

	btVector3 inertia(0, 0, 0);

	/*if (mass != 0.0) {

	boxShape->calculateLocalInertia(mass, inertia);
	}*/

	btMotionState* motion = new btDefaultMotionState(transform);

	// Definition of the rigid body
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, boxShape, inertia);

	// Create the rigid body
	btRigidBody* playerRigidBody = new btRigidBody(info);

	// Set the rigid body to the current platform 
	this->rigidBody = playerRigidBody;

	// Add the new rigid body to the dynamic world
	bulletPhysicsHandler.bulletDynamicsWorld->addRigidBody(playerRigidBody);
	bulletPhysicsHandler.rigidBodies.push_back(playerRigidBody);
}

void CharacterBase::draw(ID3D11DeviceContext* &graphicDeviceContext, int vertexCount) {

	ID3D11ShaderResourceView* nullSRV = nullptr;

	graphicDeviceContext->PSSetShaderResources(0, 1, &nullSRV);

	//graphicDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexSize, &offset);
	graphicDeviceContext->Draw(vertexCount, 0);

}

//-----------------------------------------------------------
//-------------Move and WorldMatrix -------------------------

void CharacterBase::updateWorldMatrix(XMFLOAT3 newPos, XMMATRIX rotation)
{
	// Prepare matrices for conversion
	XMMATRIX transform;
	XMFLOAT4X4 data;

	// Gather the rigid body matrix
	btTransform btRigidTransform;
	this->rigidBody->getMotionState()->getWorldTransform(btRigidTransform);
	
	// Load it into an XMFLOAT4x4
	btRigidTransform.getOpenGLMatrix((float*)&data);

	// Load it into an XMMATRIX
	transform = XMLoadFloat4x4(&data);

	XMMATRIX scale = XMMatrixScaling(0.2, 0.2, 0.2);

	transform = XMMatrixMultiply(scale, transform);

	// Build the new world matrix
	tPlayerTranslation = XMMatrixMultiply(rotation, transform);

}



void CharacterBase::resetWorldMatrix()
{
	//this->tPlayerTranslation = XMMatrixIdentity();
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
