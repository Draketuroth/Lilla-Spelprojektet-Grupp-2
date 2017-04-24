#pragma once
#include "MainCharacter.h"
class Enemy: public CharacterBase
{
public:
	Enemy();
	Enemy(int Type, const XMFLOAT3 SpawnPos);
	~Enemy();

	
	int getType()const;
	void setType(const int Type);
	XMFLOAT3 getSpawnPos()const;
	void setSpawnPos(XMFLOAT3 spawnPos);

	void Spawn(ID3D11Device* graphicDevice, BulletComponents &bulletPhysicsHandle);
	void EnemyPhysics();

	void meleeAttack(float distance, MainCharacter player);
	void rangedAttack(float distance, MainCharacter player);
	
private:
	ID3D11Buffer* gEnemieVertexBuffer;
	int Type;
	XMFLOAT3 SpawnPos;
	vector<TriangleVertex>vertices;
	vector<unsigned int>indices;

	bool attacking;
	float attackTimer;
	float attackCd;


};

