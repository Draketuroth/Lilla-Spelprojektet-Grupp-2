#ifndef ENEMIES_H
#define ENEMIES_H
#include "CharacterBase.h"

struct projectile
{
	XMMATRIX worldMatrix;
	XMFLOAT3 projectileRigidBodyExtents;
	btRigidBody* projectileRigidBody;
	

};

class Enemy: public CharacterBase
{
public:
	Enemy();
	Enemy(int Type, const XMFLOAT3 SpawnPos);
	~Enemy();

	void releaseAll(btDynamicsWorld* bulletDynamicsWorld);

	projectile fireBall;

	float getAngle(XMFLOAT3 playerPos);
	int getType()const;
	void setType(const int Type);
	XMFLOAT3 getSpawnPos()const;
	void setSpawnPos(XMFLOAT3 spawnPos);

	void Spawn(ID3D11Device* graphicDevice, BulletComponents &bulletPhysicsHandle, int enemyIndex);
	void EnemyPhysics(XMFLOAT3 playerPos, XMMATRIX scaling);

	void moveTowardsPosition(XMFLOAT3 position);
	void avoidPlayer(XMFLOAT3 position);

	void createProjectile(BulletComponents &bulletPhysicsHandler);
	void shootProjectile(float forceVx, float forceVy, XMFLOAT3 direction);

	void updateProjectile();

	int currentAnimIndex;

	bool attacking;
	bool attackFlag;

	bool rangedAttack;
	float rangedTimer;
	float rangedCd;

private:
	
	int Type;
	XMFLOAT3 SpawnPos;

};

#endif