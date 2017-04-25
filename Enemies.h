#ifndef ENEMIES_H
#define ENEMIES_H
#include "CharacterBase.h"

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

	void moveTowardsPosition(XMFLOAT3 position);
	void avoidPosition(XMFLOAT3 position);
	
private:
	ID3D11Buffer* gEnemieVertexBuffer;
	int Type;
	XMFLOAT3 SpawnPos;
	vector<TriangleVertex>vertices;
	vector<unsigned int>indices;



};

#endif