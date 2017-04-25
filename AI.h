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

	void iceAI(MainCharacter player, Enemy self);
	void fireAI(MainCharacter player, Enemy self);

	void attackMelee(float distance, MainCharacter player);
	void attackRanged(float distance, MainCharacter player);

	void moveTowardsPlayer(MainCharacter player, Enemy self);
	void moveAwayFromPlayer(MainCharacter player, Enemy self);

	float getDistance(XMFLOAT3 playerPos, XMFLOAT3 enemyPos);

	

};


#endif