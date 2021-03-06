#include "Enemies.h"


Enemy::~Enemy()
{

}

Enemy::Enemy(int Type, XMFLOAT3 SpawnPos)
	:CharacterBase(true, 5, 5.0f, 1,this->getPos(), XMMatrixIdentity())
{
	this->timer.initialize();

	this->rangedAttack = false;
	this->rangedTimer = 0.0f;
	this->rangedCd = 1.0f;
	
	this->Type = Type;
	this->setPos(SpawnPos);

	soundBuffer.loadFromFile("Sounds//enemyThrow.wav");
	enemyThrow.setBuffer(soundBuffer);
	
}

void Enemy::releaseAll(btDynamicsWorld* bulletDynamicsWorld) {

	// Removing the rigid body from the world should cause more memory leaks in the Bullet Release...we just want to deactive the rigidbody
	this->rigidBody->setActivationState(WANTS_DEACTIVATION);
	//bulletDynamicsWorld->removeCollisionObject(this->rigidBody);
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

void Enemy::Spawn(ID3D11Device* graphicDevice, BulletComponents &bulletPhysicsHandle, int enemyIndex, float radius, float height)
{
	currentAnimIndex = 0;
	CreateEnemyBoundingBox(0.10, this->getPos(), radius, height, bulletPhysicsHandle, enemyIndex);
	this->rigidBody->setIslandTag(characterRigid);//This is for checking intersection ONLY between the projectile of the player and any possible enemy, not with platforms or other rigid bodies
	
}

void Enemy::EnemyPhysics(XMFLOAT3 playerPos, XMMATRIX scaling)
{

	if (attackFlag == true) {

		currentAnimIndex = 3;
	}

	XMFLOAT3 pos;

	float time = timer.getDeltaTime();

	XMFLOAT3 lookat = { playerPos.x, 0 , playerPos.z };
	XMVECTOR positionVec = XMLoadFloat3(&this->getPos());
	XMFLOAT3 oldpos = { this->getPos().x, 0, this->getPos().z };
	
	// Calculate rotation to player
	XMVECTOR target = XMLoadFloat3(&lookat);
	XMVECTOR enemyPos = XMLoadFloat3(&oldpos);
	XMVECTOR upVector = { 0.0f, 1.0f, 0.0f };
	
	// Calculate rotation matrix axis values
	XMVECTOR zAxis = XMVector3Normalize(XMVectorSubtract(target, enemyPos));
	XMVECTOR xAxis = XMVector3Normalize(XMVector3Cross(upVector, zAxis));
	XMVECTOR yAxis = XMVector3Cross(zAxis, xAxis);

	// Important not to forget the w component of the matrix to allow translation
	XMVECTOR lastRow = { 0.0f, 0.0f, 0.0f , 1.0f};

	// Set rotation matrix
	
	XMMATRIX R = XMMATRIX(xAxis, yAxis, zAxis, lastRow);


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

	this->rigidBody->applyCentralForce(direction );


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
	XMFLOAT3 projectilePos = { this->getPos().x, -20, this->getPos().z };
	XMFLOAT3 extents = { 0.3f, 0.3f, 0.3f };

	XMMATRIX translation = XMMatrixTranslation(projectilePos.x, projectilePos.y, projectilePos.z);
	XMFLOAT4X4 t;
	XMStoreFloat4x4(&t, translation);

	btTransform transform;
	transform.setFromOpenGLMatrix((float*)&t);

	btSphereShape* sphereShape = new btSphereShape(1);
	btVector3 inertia(0, 0, 0);

	btMotionState* motion = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo info(0.1, motion, sphereShape, inertia);

	fireBall.projectileRigidBodyExtents = extents;

	fireBall.projectileRigidBody = new btRigidBody(info);
	fireBall.projectileRigidBody->setActivationState(DISABLE_DEACTIVATION);

	int projCollideWith = COL_PLAYER | COL_LEVEL;
	bulletPhysicsHandler.bulletDynamicsWorld->addRigidBody(fireBall.projectileRigidBody, COL_PROJ, projCollideWith);
	bulletPhysicsHandler.projectileRigidBodies.push_back(fireBall.projectileRigidBody);

}

void Enemy::shootProjectile(float forceVx, float forceVy, XMFLOAT3 direction)
{

	// If enemy is not attacking, get back the projectile
	if (!this->rangedAttack)
	{
		btTransform transform = fireBall.projectileRigidBody->getCenterOfMassTransform();
		transform.setIdentity();
		transform.setOrigin(btVector3(getPos().x, getPos().y + 1.5f, getPos().z));
		fireBall.projectileRigidBody->setWorldTransform(transform);
	}

	// If enemy is attacking, remove projectile after a given time
	else if (this->rangedTimer < 0.7 && this->rangedAttack) {

		btTransform projectileTransform;
		projectileTransform.setIdentity();
		projectileTransform.setOrigin(btVector3(0, -100, 0));
		fireBall.projectileRigidBody->setWorldTransform(projectileTransform);
	}

	if (!this->rangedAttack && this->rangedTimer <= 0)
	{
		enemyThrow.play();

		this->rangedAttack = true;
		this->rangedTimer = rangedCd;
	
		float forceVz = forceVx * direction.z ;
		forceVx = forceVx * direction.x;
		forceVy *= 1.3f;

		btVector3 force = { forceVx, forceVy, forceVz };

		fireBall.projectileRigidBody->applyCentralForce(force);
		fireBall.projectileRigidBody->setFriction(3);
	}

	if (this->rangedAttack)
	{
		if (this->rangedTimer > 0){

			this->rangedTimer -= timer.getDeltaTime();

		}

		else
		{

			this->rangedAttack = false;
			this->attackFlag = false;

		}
	}

	timer.updateCurrentTime();
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

	// Scale
	XMMATRIX scaling = XMMatrixScaling(0.6f, 0.6f, 0.6f);

	// Build the new world matrix
	fireBall.worldMatrix = XMMatrixMultiply(scaling, transform);
	
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