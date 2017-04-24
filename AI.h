#ifndef AI_H
#define AI_H

#include "MainCharacter.h"

class AI
{
private:


public:
	AI();
	~AI();

	void iceAI(MainCharacter player, Enemy self);
	void fireAI(MainCharacter player, Enemy self);

	float getDistance(XMFLOAT3 playerPos, XMFLOAT3 enemyPos);


};


#endif