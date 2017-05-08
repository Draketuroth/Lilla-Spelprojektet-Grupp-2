#include "Enemies.h"

Enemy::Enemy()
	:CharacterBase()
{
	this->Type = 0;
	this->SpawnPos = { 0,0,0 };

}

Enemy::~Enemy()
{

}

Enemy::Enemy(int Type,XMFLOAT3 SpawnPos)
	:CharacterBase(true, 5, 5.0f, 1,this->getPos(), XMMatrixIdentity())
{
	
	this->Type = Type;
	this->setPos(SpawnPos);
	
}

float Enemy::getAngle(XMFLOAT3 playerPos)
{
	float angle;
	
	float playerX = (2 * playerPos.x) / WIDTH - 1;
	float playerZ = (2 * playerPos.z) / HEIGHT - 1;

	playerZ *= -1;

	angle = atan2(playerX, playerZ);

	return angle;
}

int Enemy::getType()const
{
	return this->Type;
}
void Enemy::setType(const int Type)
{
	this->Type = Type;
}
XMFLOAT3 Enemy::getSpawnPos()const
{
	return this->SpawnPos;
}
void Enemy::setSpawnPos(XMFLOAT3 SpawnPos)
{
	this->SpawnPos = SpawnPos;
}

void Enemy::Spawn(ID3D11Device* graphicDevice, BulletComponents &bulletPhysicsHandle, FileImporter &importer)
{
	
	loadVertices(importer, graphicDevice);
	createBuffer(graphicDevice, vertices);
	CreateBoundingBox(0.10, this->getPos(), XMFLOAT3(1, 1, 1), bulletPhysicsHandle);
	this->rigidBody->setIslandTag(characterRigid);//This is for checking intersection ONLY between the projectile of the player and any possible enemy, not with platforms or other rigid bodies
	
}

void Enemy::loadVertices(FileImporter &importer, ID3D11Device* &graphicDevice) {

	HRESULT hr;
	//load mesh vertices
	for (UINT i = 0; i < importer.skinnedMeshes[0].vertices.size(); i++) {

		StandardVertex vertex;

		vertex.x = importer.skinnedMeshes[0].vertices[i].pos[0];
		vertex.y = importer.skinnedMeshes[0].vertices[i].pos[1];
		vertex.z = importer.skinnedMeshes[0].vertices[i].pos[2];

		vertex.u = importer.skinnedMeshes[0].vertices[i].uv[0];
		vertex.v = importer.skinnedMeshes[0].vertices[i].uv[1];

		vertex.nx = importer.skinnedMeshes[0].vertices[i].normal[0];
		vertex.ny = importer.skinnedMeshes[0].vertices[i].normal[1];
		vertex.nz = importer.skinnedMeshes[0].vertices[i].normal[2];

		vertices.push_back(vertex);

	}
}

void Enemy::EnemyPhysics()
{
	XMFLOAT3 pos;

	float time = timer.getDeltaTime();


	XMVECTOR positionVec = XMLoadFloat3(&this->getPos());
	XMFLOAT3 oldpos = this->getPos();

	XMMATRIX R = XMMatrixIdentity();
	XMMATRIX scaling = XMMatrixScaling(0.1, 0.1, 0.1);
	updateWorldMatrix(R, scaling);
	
	XMMATRIX transform;
	XMFLOAT4X4 data;

	// Gather the rigid body matrix
	btTransform btRigidTransform;
	this->rigidBody->getMotionState()->getWorldTransform(btRigidTransform);

	// Load it into an XMFLOAT4x4
	btRigidTransform.getOpenGLMatrix((float*)&data);

	// Load it into an XMMATRIX
	transform = XMLoadFloat4x4(&data);
	XMVECTOR t;
	XMVECTOR s;
	XMVECTOR r;
	XMMatrixDecompose(&s, &r, &t, transform);
	XMFLOAT3 rigidPos;
	XMStoreFloat3(&rigidPos, t);

	this->setPos(rigidPos);

	

	

	timer.updateCurrentTime();
}

void Enemy::moveTowardsPosition(XMFLOAT3 position)
{
	XMFLOAT3 myPos = this->getPos();
	btVector3 positionAt = { myPos.x, myPos.y, myPos.z };
	btVector3 positionTarget = { position.x, position.y, position.z };
	btVector3 direction = positionTarget - positionAt;
	direction.normalize();

	this->rigidBody->applyCentralForce(direction * 0.5);


	//This is the speed of the enemy
	//May be tweaked
	float maxSpeed = 3;
	float minSpeed = -3;
	btVector3 speed = this->rigidBody->getLinearVelocity();

	//X-led
	if (speed.getX() > maxSpeed)
	{
		speed.setX(maxSpeed);
	}
	if (speed.getX() < minSpeed)
	{
		speed.setX(minSpeed);
	}
	//Z-led
	if (speed.getZ() > maxSpeed)
	{
		speed.setZ(maxSpeed);
	}
	if (speed.getZ() < minSpeed)
	{
		speed.setZ(minSpeed);
	}

	this->rigidBody->setLinearVelocity(speed);

}

void Enemy::avoidPlayer(XMFLOAT3 position)
{
	XMFLOAT3 myPos = this->getPos();
	btVector3 positionAt = { myPos.x, myPos.y, myPos.z };
	btVector3 positionTarget = { position.x, position.y, position.z };
	btVector3 direction = positionTarget - positionAt;
	direction.normalize();

	this->rigidBody->applyCentralForce(direction * -0.5);


	//This is the speed of the enemy
	//May be tweaked
	float maxSpeed = 3;
	float minSpeed = -3;
	btVector3 speed = this->rigidBody->getLinearVelocity();

	//X-led
	if (speed.getX() > maxSpeed)
	{
		speed.setX(maxSpeed);
	}
	if (speed.getX() < minSpeed)
	{
		speed.setX(minSpeed);
	}
	//Z-led
	if (speed.getZ() > maxSpeed)
	{
		speed.setZ(maxSpeed);
	}
	if (speed.getZ() < minSpeed)
	{
		speed.setZ(minSpeed);
	}

	this->rigidBody->setLinearVelocity(speed);
}

void Enemy::createProjectile(BulletComponents &bulletPhysicsHandler)
{
	XMFLOAT3 projectilePos = { this->getPos().x, this->getPos().y, this->getPos().z };
	XMFLOAT3 extents = { 0.1f, 0.1f, 0.1f };

	XMMATRIX translation = XMMatrixTranslation(projectilePos.x, projectilePos.y, projectilePos.z);
	XMFLOAT4X4 t;
	XMStoreFloat4x4(&t, translation);

	btTransform transform;
	transform.setFromOpenGLMatrix((float*)&t);

	btBoxShape* boxShape = new btBoxShape(btVector3(extents.x, extents.y, extents.z));
	btVector3 inertia(0, 0, 0);

	btMotionState* motion = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo info(0.1, motion, boxShape, inertia);

	fireBall.projectileRigidBodyExtents = extents;

	fireBall.projectileRigidBody = new btRigidBody(info);
	fireBall.projectileRigidBody->setActivationState(DISABLE_DEACTIVATION);

	bulletPhysicsHandler.bulletDynamicsWorld->addRigidBody(fireBall.projectileRigidBody);
	bulletPhysicsHandler.rigidBodies.push_back(fireBall.projectileRigidBody);

	

}
void Enemy::shootProjectile(float forceVx, float forceVy, float forward)
{
	//apply the force to the projectile

	// v0y is the force in Y
	// v0x is the force FORWARD

	forceVx = forceVx * forward;

	fireBall.projectileRigidBody->applyCentralForce(btVector3(forceVx / 2, forceVy, forceVx / 2));



	//how to get the right direction?
}

bool Enemy::createProjectileBox(ID3D11Device* gDevice)
{
	HRESULT hr;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// HARDCODED VERTICES
	//----------------------------------------------------------------------------------------------------------------------------------//

	TriangleVertex Projectile[24] =
	{

		//Front face

		-0.1f, 0.1f, -0.1f, 0.0f, 0.0f,
		0.1f, 0.1f, -0.1f, 0.1f, 0.0f,
		-0.1f, -0.1f, -0.1f, 0.0f, 0.1f,
		0.1f, -0.1f, -0.1f, 0.1f, 0.1f,

		// Back face

		0.1f, 0.1f, 0.1f, 0.0f, 0.0f,
		-0.1f, 0.1f, 0.1f, 0.1f, 0.0f,
		0.1f, -0.1f, 0.1f, 0.0f, 0.1f,
		-0.1f, -0.1f, 0.1f, 0.1f, 0.1f,

		// Left face

		-0.1f, 0.1f,   0.1f, 0.0f, 0.0f,
		-0.1f, 0.1f, - 0.1f, 0.1f, 0.0f,
		-0.1f, -0.1f,  0.1f, 0.0f, 0.1f,
		-0.1f, -0.1f, -0.1f, 0.1f, 0.1f,

		// Right face

		0.1f, 0.1f, -0.1f, 0.0f, 0.0f,
		0.1f, 0.1f, 0.1f, 0.1f, 0.0f,
		0.1f, -0.1f, -0.1f, 0.0f, 0.1f,
		0.1f, -0.1f,  0.1f, 0.1f, 0.1f,

		// Top face

		-0.1f, 0.1f, 0.1f, 0.0f, 0.0f,
		0.1f, 0.1f, 0.1f, 0.1f, 0.0f,
		-0.1f, 0.1f, -0.1f, 0.0f, 0.1f,
		0.1f, 0.1f, -0.1f, 0.1f, 0.1f,

		// Bottom face

		0.1f, -0.1f, 0.1f, 0.0f, 0.0f,
		-0.1f, -0.1f, 0.1f, 0.1f, 0.0f,
		0.1f, -0.1f, -0.1f, 0.0f, 0.1f,
		-0.1f, -0.1f, -0.1f, 0.1f, 0.1f


	};

	// Create Indices
	unsigned int ProjectileIndicies[36] =
	{

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

	//----------------------------------------------------------------------------------------------------------------------------------//
	// CREATE VERTEX BUFFER
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Projectile);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = Projectile;
	hr = gDevice->CreateBuffer(&bufferDesc, &data, &gProjectileBuffer);

	if (FAILED(hr)) 
	{

		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// CREATE INDEX BUFFER
	//----------------------------------------------------------------------------------------------------------------------------------//

	// Create the buffer description
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(unsigned int) * 36;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Set the subresource data

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &ProjectileIndicies[0];
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Create the buffer

	hr = gDevice->CreateBuffer(&bufferDesc, &initData, &gProjectileIndexBuffer);

	if (FAILED(hr))
	{

		return false;
	}

	return true;
}

void Enemy::updateProjectile()
{
	XMMATRIX transform;
	XMFLOAT4X4 data;

	// Gather the rigid body matrix
	btTransform btRigidTransform;
	fireBall.projectileRigidBody->getMotionState()->getWorldTransform(btRigidTransform);

	// Load it into an XMFLOAT4x4
	btRigidTransform.getOpenGLMatrix((float*)&data);

	// Load it into an XMMATRIX
	transform = XMLoadFloat4x4(&data);

	// Build the new world matrix
	fireBall.worldMatrix = transform;
	

}



//void Enemy::meleeAttacks(float distance)
//{
//	if (!attacking && attackTimer <= 0)
//	{
//		attacking = true;
//		attackTimer = attackCd;
//
//		if (distance <= 2)
//		{
//			//player.setHealth(getHealth() - 1);
//		}
//	}
//
//	if (attacking)
//	{
//		if (attackTimer > 0)
//			attackTimer -= timer.getDeltaTime();
//		else
//		{
//			attacking = false;
//		}
//	}
//
//}
//void Enemy::rangedAttack(float distance)
//{
//	if (!attacking && attackTimer <= 0)
//	{
//		attacking = true;
//		attackTimer = attackCd;
//
//		if (distance <= 8)
//		{
//			//Attack with a ranged attack
//		}
//	}
//
//	if (attacking)
//	{
//		if (attackTimer > 0)
//			attackTimer -= timer.getDeltaTime();
//		else
//		{
//			attacking = false;
//		}
//	}
//}