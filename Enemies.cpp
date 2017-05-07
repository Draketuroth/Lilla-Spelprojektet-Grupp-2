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