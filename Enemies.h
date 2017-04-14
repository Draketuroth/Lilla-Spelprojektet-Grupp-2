#pragma once
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

	void Spawn();
	struct EnemyCube
	{
		XMFLOAT3 VPos[33];
	};
private:
	ID3D11Buffer* gEnemieVertexBuffer;
	int Type;
	XMFLOAT3 SpawnPos;



};

