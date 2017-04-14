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
	:CharacterBase(true, 2, 5.0f, 1, { 1,1,1 }, XMMatrixIdentity())
{
	
	this->Type = Type;
	this->SpawnPos = SpawnPos;
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

void Enemy::Spawn()
{
	
	EnemyCube HostileCube;

	HostileCube.VPos[0] = { 1,1,1 };
	HostileCube.VPos[1] = { 1,1,-1 }; //1
	HostileCube.VPos[2] = { 1,-1,1 };

	HostileCube.VPos[3] = { 1,1,-1 };
	HostileCube.VPos[4] = { 1,-1,1 }; //2
	HostileCube.VPos[5] = { 1,-1,-1 };

	HostileCube.VPos[4] = { 1,-1,1 };
	HostileCube.VPos[5] = { 1,-1,-1 }; //3
	HostileCube.VPos[6] = { -1,-1,-1 };

	HostileCube.VPos[7] = { 1,-1,1 };
	HostileCube.VPos[8] = { -1,-1,-1 }; //4
	HostileCube.VPos[9] = { -1,-1,1 };

	HostileCube.VPos[10] = { 1,-1,1 };
	HostileCube.VPos[11] = { -1,-1,1 }; //5
	HostileCube.VPos[12] = { -1,1,1 };

	HostileCube.VPos[13] = { 1,1,1 };
	HostileCube.VPos[14] = { -1,1,1 }; //6
	HostileCube.VPos[15] = { 1,-1,1 };


	HostileCube.VPos[16] = { 1,1,1 };
	HostileCube.VPos[17] = { -1,1,1 }; //7
	HostileCube.VPos[18] = { 1,1,-1 };

	HostileCube.VPos[19] = { -1,1,1 };
	HostileCube.VPos[20] = { 1,1,-1 };//8
	HostileCube.VPos[21] = { -1,1,-1 };

	HostileCube.VPos[22] = { 1,1,-1 };
	HostileCube.VPos[23] = { -1,1,-1 };//9
	HostileCube.VPos[24] = { 1,-1,-1 };

	HostileCube.VPos[25] = { 1,-1,-1 };
	HostileCube.VPos[26] = { -1,-1,-1 };//10
	HostileCube.VPos[27] = { -1,1,-1 };

	HostileCube.VPos[28] = { -1,1,-1 };
	HostileCube.VPos[29] = { -1,-1,-1 };//11
	HostileCube.VPos[30] = { -1,1,1 };

	HostileCube.VPos[31] = { -1,-1,-1 };
	HostileCube.VPos[32] = { -1,1,1 };//12
	HostileCube.VPos[33] = { -1,-1,1 };


	for (int i = 0; i < 33; i++)
	{
		HostileCube.VPos[i].x += this->SpawnPos.x;
		HostileCube.VPos[i].y += this->SpawnPos.y;
		HostileCube.VPos[i].z += this->SpawnPos.z;
	}



	/*D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(HostileCube);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &HostileCube;
	hr = gDevice->CreateBuffer(&bufferDesc, &data, &gEnemieVertexBuffer);*/


}