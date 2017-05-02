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

void Enemy::Spawn(ID3D11Device* graphicDevice, BulletComponents &bulletPhysicsHandle)
{
	
	TriangleVertex HostileCube[24] =
	{
		//Vtx		UV
		-1,1,-1,	0.0f,0.0f,
		1,1,-1,		1.0f,0.0f,
		-1,-1,-1,	0.0f,1.0f, //Front Face
		1,-1,-1,	0.0f,0.0f,

		1,1,1,		0.0f,0.0f,//BackFace
		-1,1,1,		1.0f,0.0f,
		1,-1,1,		0.0f,1.0f,
		-1,-1,1,	1.0f,1.0f,

		-1,1,1,		0.0f,0.0f,//LeftFace
		-1,1,-1,	1.0f,0.0f,
		-1,-1,1,	0.0f,1.0f,
		-1,-1,-1,	1.0f,1.0f,

		1,1,-1,		0.0f,0.0f,//RightFace
		1,1,1,		1.0f,0.0f,
		1,-1,-1,	0.0f,1.0f,
		1,-1,1,		1.0f,1.0f,

		-1,1,1,		0.0f,0.0f,//TopFace
		1,1,1,		1.0f,0.0f,
		-1,1,-1,	0.0f,1.0f,
		1,1,-1,		1.0f,1.0f,

		1,-1,1,		0.0f,0.0f,//BottomFace
		-1,-1,1,	1.0f,0.0f,
		1,-1,-1,	0.0f,1.0f,
		-1,-1,-1,	1.0f,1.0f

	};

	for (unsigned int i = 0; i < 24; i++)
	{

		vertices.push_back(HostileCube[i]);
	}

	// Create Indices
	unsigned int HostileCubeIndicies[36] = 
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

	for (unsigned int i = 0; i < 36; i++) {

		indices.push_back(HostileCubeIndicies[i]);
	}
	
	createBuffers(graphicDevice, vertices, indices);
	CreateBoundingBox(0.10, this->getPos(), XMFLOAT3(1, 1, 1), bulletPhysicsHandle);
	this->rigidBody->setIslandTag(characterRigid);//This is for checking intersection ONLY between the projectile of the player and any possible enemy, not with platforms or other rigid bodies
	
}

void Enemy::EnemyPhysics()
{
	XMFLOAT3 pos;

	float time = timer.getDeltaTime();


	XMVECTOR positionVec = XMLoadFloat3(&this->getPos());
	XMFLOAT3 oldpos = this->getPos();

	XMMATRIX R = XMMatrixIdentity();
	updateWorldMatrix(R);

	

	
	
	
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



	//float maxSpeed = 2;
	//float minSpeed = -2;
	//btVector3 speed = this->rigidBody->getLinearVelocity();

	////X-led
	//if (speed.getX() > maxSpeed)
	//{
	//	speed.setX(maxSpeed);
	//}
	//if (speed.getX() < minSpeed)
	//{
	//	speed.setX(minSpeed);
	//}
	////Z-led
	//if (speed.getZ() > maxSpeed)
	//{
	//	speed.setZ(maxSpeed);
	//}
	//if (speed.getZ() < minSpeed)
	//{
	//	speed.setZ(minSpeed);
	//}
}

void Enemy::avoidPosition(XMFLOAT3 position)
{
	//implement
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