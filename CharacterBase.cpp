#include "CharacterBase.h"
#include <sstream>

CharacterBase::CharacterBase()
{
	this->health = 0;
	this->movementSpeed = 20;
	this->unitID = 0;
	this->alive = true;
	this->timer.initialize();
	this->forwardVector = { 0, 0, 1 };
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
	this->forwardVector = { 0, 0, 1 };
}

CharacterBase::~CharacterBase()
{
	//Safe release
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
bool CharacterBase::createBuffers(ID3D11Device* &graphicDevice, vector<Vertex_Bone>vertices, AnimationHandler &animHandler, CHARACTER_SKINNED_DATA &skinData)
{
	HRESULT hr;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// BONE BUFFER DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_BUFFER_DESC boneBufferDesc;

	memset(&boneBufferDesc, 0, sizeof(boneBufferDesc));

	boneBufferDesc.ByteWidth = sizeof(CHARACTER_SKINNED_DATA);
	boneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	boneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	boneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	boneBufferDesc.MiscFlags = 0;
	boneBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA boneData;
	boneData.pSysMem = &skinData;
	boneData.SysMemPitch = 0;
	boneData.SysMemSlicePitch = 0;

	hr = graphicDevice->CreateBuffer(&boneBufferDesc, &boneData, &animHandler.gCharacterBoneBuffer);

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
	bufferDesc.ByteWidth = vertices.size() * sizeof(Vertex_Bone);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &vertices[0];
	hr = graphicDevice->CreateBuffer(&bufferDesc, &data, &vertexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

void CharacterBase::CreatePlayerBoundingBox(float mass, XMFLOAT3 spawnPos, float radius, float height, BulletComponents &bulletPhysicsHandler) {

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
	//btBoxShape* boxShape = new btBoxShape(btVector3(extents.x, extents.y, extents.z));
	//boxShape->setMargin(0.04);

	btCapsuleShape* capsule = new btCapsuleShape(radius, height);
	//capsule->setMargin(0.04);


	btVector3 inertia(0, 0, 0);

	/*if (mass != 0.0) {

	boxShape->calculateLocalInertia(mass, inertia);
	}*/

	btMotionState* motion = new btDefaultMotionState(transform);

	// Definition of the rigid body
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, capsule, inertia);

	//this->boundingBoxExtents = capsule;

	// Create the rigid body
	btRigidBody* playerRigidBody = new btRigidBody(info);
	
	// Set the rigid body to the current platform 
	this->rigidBody = playerRigidBody;
	this->rigidBody->setActivationState(DISABLE_DEACTIVATION);

	// Add the new rigid body to the dynamic world
	int playerCollideWith = COL_ENEMY | COL_LEVEL | COL_PROJ;
	bulletPhysicsHandler.bulletDynamicsWorld->addRigidBody(playerRigidBody, COL_PLAYER, playerCollideWith);
	bulletPhysicsHandler.rigidBodies.push_back(playerRigidBody);
	
}

void CharacterBase::CreateEnemyBoundingBox(float mass, XMFLOAT3 spawnPos, float radius, float height, BulletComponents &bulletPhysicsHandler, int enemyIndex) {

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
	//btBoxShape* boxShape = new btBoxShape(btVector3(extents.x, extents.y, extents.z));
	//boxShape->setMargin(0.04);

	btCapsuleShape* capsule = new btCapsuleShape(radius, height); 
	
	btVector3 inertia(0, 0, 0);

	/*if (mass != 0.0) {

	boxShape->calculateLocalInertia(mass, inertia);
	}*/

	btMotionState* motion = new btDefaultMotionState(transform);

	// Definition of the rigid body
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, capsule, inertia);

	this->boundingBoxExtents = XMFLOAT3(0.5, 0.8, 0.3);

	// Create the rigid body
	btRigidBody* enemyRigidBody = new btRigidBody(info);
	enemyRigidBody->setUserIndex(enemyIndex);

	// Set the rigid body to the current platform 
	this->rigidBody = enemyRigidBody;
	this->rigidBody->setActivationState(DISABLE_DEACTIVATION);

	// Add the new rigid body to the dynamic world
	int enemyCollideWith = COL_PLAYER | COL_LEVEL | COL_ENEMY | COL_RAY;//Flags to determine what objects the enemy can collide with
	bulletPhysicsHandler.bulletDynamicsWorld->addRigidBody(enemyRigidBody, COL_ENEMY, enemyCollideWith);
	bulletPhysicsHandler.enemyRigidBodies.push_back(enemyRigidBody);

}

BoundingBox CharacterBase::getBoundingBox()
{
	//we need the center and the extents, both as XMFLOAT3
	XMFLOAT3 center = this->position;
	XMFLOAT3 extents = this->boundingBoxExtents;
	return BoundingBox(center, extents);
}

// onödig funktion ?
btCapsuleShape CharacterBase::getBoundingCapsule()
{
	float radius = 0.0; 
	float height = 0.0; 
	return btCapsuleShape(radius, height); 
}

XMMATRIX CharacterBase::getPlayerTanslationMatrix()
{
	return this->tPlayerTranslation;
}


void CharacterBase::draw(ID3D11DeviceContext* &graphicDeviceContext, int vertexCount) {

	graphicDeviceContext->Draw(vertexCount, 0);

}

void CharacterBase::draw(ID3D11DeviceContext* &graphicDeviceContext) {

	UINT32 vertexSize = sizeof(TriangleVertex);
	UINT32 offset = 0;

	graphicDeviceContext->DrawIndexed(36, 0, 0);
}

//-----------------------------------------------------------
//-------------Move and WorldMatrix -------------------------

void CharacterBase::updateWorldMatrix(XMMATRIX rotation)
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

	// Build the new world matrix
	tPlayerTranslation = XMMatrixMultiply(rotation, transform);

	this->forwardVector = XMVector3Transform(XMVECTOR{ 0,0,1 }, rotation);

}

void CharacterBase::updateWorldMatrix(XMMATRIX rotation, XMMATRIX scale)
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

	transform = XMMatrixMultiply(scale, transform);

	// Build the new world matrix
	tPlayerTranslation = XMMatrixMultiply(rotation, transform);

}

XMVECTOR CharacterBase::getForwardVector()
{
	return this->forwardVector;
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

