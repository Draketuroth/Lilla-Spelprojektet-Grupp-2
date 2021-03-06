#ifndef AI_H
#define AI_H

#include "Enemies.h"
#include "MainCharacter.h"
#include "Timer.h"

class AI
{
private:
	bool attacking;
	float attackTimer;
	float attackCd;

	Timer timer;

public:
	AI();
	~AI();

	void iceAI(MainCharacter &player, Enemy* self, float enemyTimePos);
	void fireAI(MainCharacter &player, Enemy* self, BulletComponents &bulletPhysicsHandler, float enemyTimePos);

	void attackMelee(MainCharacter &player, Enemy* self, float enemyTimePos);
	void attackRanged(MainCharacter &player, Enemy* self, BulletComponents &bulletPhysicsHandler, float enemyTimePos);

	void moveTowardsPlayer(XMFLOAT3 playerPosition, Enemy *self);
	void moveAwayFromPlayer(XMFLOAT3 playerPosition, Enemy *self);

	float getDistance(XMFLOAT3 playerPos, XMFLOAT3 enemyPos);

	XMMATRIX rotate(XMFLOAT3 playerPos, Enemy &self);

	btVector3 collisionEdge(BoundingBox sides[], Enemy* self);

	sf::Sound meleeSound;
	sf::SoundBuffer soundBuffer;

};


#endif