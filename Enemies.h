#ifndef ENEMIES_H
#define ENEMIES_H
#include "CharacterBase.h"

class Enemy: public CharacterBase
{
public:
	Enemy();
	Enemy(int Type, const XMFLOAT3 SpawnPos);
	~Enemy();

	float getAngle(XMFLOAT3 playerPos);
	int getType()const;
	void setType(const int Type);
	XMFLOAT3 getSpawnPos()const;
	void setSpawnPos(XMFLOAT3 spawnPos);

	void Spawn(ID3D11Device* graphicDevice, BulletComponents &bulletPhysicsHandle, FileImporter &importer);
	void loadVertices(FileImporter &importer, ID3D11Device* &graphicDevice);
	void EnemyPhysics();

	void moveTowardsPosition(XMFLOAT3 position);
	void avoidPlayer(XMFLOAT3 position);
	
	vector<StandardVertex>vertices;
private:
	ID3D11Buffer* gEnemieVertexBuffer;
	int Type;
	XMFLOAT3 SpawnPos;
	vector<unsigned int>indices;



};

#endif