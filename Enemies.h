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

	projectile fireBall;

	float getAngle(XMFLOAT3 playerPos);
	int getType()const;
	void setType(const int Type);
	XMFLOAT3 getSpawnPos()const;
	void setSpawnPos(XMFLOAT3 spawnPos);

	void Spawn(ID3D11Device* graphicDevice, BulletComponents &bulletPhysicsHandle);
	void EnemyPhysics();

	void moveTowardsPosition(XMFLOAT3 position);
	void avoidPlayer(XMFLOAT3 position);

	void createProjectile(BulletComponents &bulletPhysicsHandler);
	void shootProjectile(float forceVx, float forceVy, float forward);

	bool createProjectileBox(ID3D11Device* gDevice);

	void updateProjectile();

	ID3D11Buffer* gEnemieVertexBuffer;
	ID3D11Buffer* gProjectileBuffer;
	ID3D11Buffer* gProjectileIndexBuffer;
	
private:
	
	int Type;
	XMFLOAT3 SpawnPos;
	vector<TriangleVertex>vertices;
	vector<unsigned int>indices;

	


};

#endif