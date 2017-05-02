#include "AI.h"

AI::AI()
{
	this->attacking = false;
	this->attackTimer = 0;
	this->attackCd = 0.7;
}
AI::~AI()
{
}

//NOTES
//we need the enemy's position to get he distance
void AI::iceAI(MainCharacter &player, Enemy &self)
{
	// vv  this has to update all the time!! Update function? vv
	float distance = getDistance(player.getPos(), self.getPos());

	if (distance <= 2)
	{
		attackMelee(player, self);
	}
	else
	{
		moveTowardsPlayer(player.getPos(), self);
	}
	//vvv ALT vvv

	//else if (self.getHealth() < 2)
	//{
	//	//move away from player
	//}
	
	//vvv Notes vvv

	/*get the player position and move towards it
	avoid holes in the ground and the edges of the arena

	when player is within melee-range -> ATTACK
	make sure the attacks have a cool down. Longer cool down than the player has!

	ice enemies won't run from the player.*/

	//NOTES:
	//We could edit this and make it so that
	//when the enemy's health is below a certain number it will flee 
	//instead of approach the enemy

}
void AI::fireAI(MainCharacter &player, Enemy &self)
{
	float distance = getDistance(player.getPos(), self.getPos());

	if (distance <= 8 && distance > 5)
	{
		//attackRanged(distance, player);
	}
	else if (distance <= 5)
	{
		moveAwayFromPlayer(player.getPos(), self);
	}
	else
	{
		moveTowardsPlayer(player.getPos(), self);
	}

	//vvv Notes vvv

	/*get the player position and move closer 
	when the player is within the range of a ranged-attack -> ATTACK

	the attack needs a cooldown slower than the player's.

	when the player gets within a certain distance -> moce away from the player
	avoid holes in the ground and the edges of the arena*/

}

void AI::attackMelee(MainCharacter &player, Enemy &self)
{
	if (!attacking && attackTimer <= 0)
	{
		attacking = true;
		attackTimer = attackCd;

		
		XMFLOAT3 enemyPos = self.getBoundingBox().Center;
		XMMATRIX enemyTranslation = self.getPlayerTanslationMatrix();

		XMVECTOR forwardVector = self.getForwardVector();
		XMFLOAT3 forward;
		XMStoreFloat3(&forward, forwardVector);

		XMFLOAT3 attackBoxCenter;

		attackBoxCenter.x = enemyPos.x * forward.x;
		attackBoxCenter.y = enemyPos.y;
		attackBoxCenter.z = enemyPos.z * forward.z;

		XMFLOAT3 attackBoxRange = { 2, 2, 2 };

		BoundingBox attackBox = BoundingBox(attackBoxCenter, attackBoxRange);
		attackBox.Transform(attackBox, enemyTranslation);
		
		BoundingBox playerBox = player.getBoundingBox();
		if (playerBox.Intersects(attackBox))
		{
			cout << "PLAYER GOT HIT" << endl;

			player.setHealth(player.getHealth() - 1);

			//Implement knockback


			if (player.getHealth() <= 0 && player.getAlive() == true)
			{
				player.setAlive(false);
				cout << "PLAYER IS DEAD" << endl;
			}
		}
	}
		
	if (attacking)
	{
		if (attackTimer > 0)
			attackTimer -= timer.getDeltaTime();
		else
		{
			attacking = false;
		}
		//play enemy attack animation here
	}
		
}
void AI::attackRanged(MainCharacter &player, Enemy &self)
{
	//if (!attacking && attackTimer <= 0)
	//{
	//	attacking = true;
	//	attackTimer = attackCd;
	//
	//	if (distance <= 8)
	//	{
	//		//ranged attack
	//	}
	//}
	//
	//if (attacking)
	//{
	//	if (attackTimer > 0)
	//		attackTimer -= timer.getDeltaTime();
	//	else
	//	{
	//		attacking = false;
	//	}
	//	//play enemy attack animation here
	//}
}

void AI::moveTowardsPlayer(XMFLOAT3 playerPosition, Enemy &self)
{
	self.moveTowardsPosition(playerPosition);
}
void AI::moveAwayFromPlayer(XMFLOAT3 playerPosition, Enemy &self)
{
	self.avoidPlayer(playerPosition);
}

float AI::getDistance(XMFLOAT3 playerPos, XMFLOAT3 enemyPos)
{
	float distance;

	float a2 = pow((playerPos.x - enemyPos.x), 2);
	float b2 = pow((playerPos.z - enemyPos.z), 2);
	
	distance = sqrt(a2 + b2);

	return distance;
}


