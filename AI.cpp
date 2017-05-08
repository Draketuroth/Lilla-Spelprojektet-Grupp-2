#include "AI.h"

AI::AI()
{
	this->attacking = true;
	this->attackTimer = 1.0;
	this->attackCd = 1.5;

	this->rangedAttack = true;
	this->rangedTimer = 10.0;
	this->rangedCd = 15.0;
	
	this->timer.initialize();
}
AI::~AI()
{
}

//NOTES
//we need the enemy's position to get he distance
void AI::iceAI(MainCharacter &player, Enemy &self)
{
	

	//AI
	float distance = getDistance(player.getPos(), self.getPos());
	if (distance <= 2)
	{
		attackMelee(player, self);
	}
	else if (self.getHealth() <= 2 && distance <= 8)
	{
		moveAwayFromPlayer(player.getPos(), self);
	}
	else if (self.getHealth() > 2 && distance > 2)
	{
		moveTowardsPlayer(player.getPos(), self);
	}


	

	timer.updateCurrentTime();

}
void AI::fireAI(MainCharacter &player, Enemy &self, BulletComponents &bulletPhysicsHandler)
{
	float distance = getDistance(player.getPos(), self.getPos());

	if (distance <= 8 && distance > 5)
	{
		attackRanged(player, self, bulletPhysicsHandler);
	}
	else if (distance <= 5)
	{
		//moveAwayFromPlayer(player.getPos(), self);
	}
	else
	{
		//moveTowardsPlayer(player.getPos(), self);
	}
}

void AI::attackMelee(MainCharacter &player, Enemy &self)
{
	//cout << "TRIED TO ATTACK" << endl;
	if (!attacking && attackTimer <= 0)
	{

		cout << "ENEMY ATTACK!" << endl;

		attacking = true;
		attackTimer = attackCd;

		//-----------------Calculate the hit area-------------------------------
		float angle = self.getAngle(player.getPos());

		XMFLOAT3 enemyPos = self.getBoundingBox().Center;
		XMVECTOR enemyDirVec = self.getForwardVector();

		XMVECTOR rotQuat = XMQuaternionRotationAxis(XMVECTOR{ 0, 1, 0 }, angle);
		enemyDirVec = XMVector3Rotate(enemyDirVec, rotQuat);
		enemyDirVec = XMVector3Normalize(enemyDirVec);

		XMVECTOR newC = XMLoadFloat3(&enemyPos);
		newC += enemyDirVec * 0.4;
		XMFLOAT3 newCenter;
		XMStoreFloat3(&newCenter, newC);

		XMFLOAT3 boxCenter = newCenter;
		XMFLOAT3 boxRange = { 2, 2, 2 };

		BoundingBox meleeBox = BoundingBox(boxCenter, boxRange);

		BoundingBox playerBox = player.getBoundingBox();
		if (playerBox.Intersects(meleeBox))
		{
			cout << "PLAYER GOT HIT" << endl;

			player.setHealth(player.getHealth() - 1);

			//Implement knockback
			btTransform playerTrans;
			btTransform enemyTrans;
			self.rigidBody->getMotionState()->getWorldTransform(enemyTrans);
			player.rigidBody->getMotionState()->getWorldTransform(playerTrans);

			btVector3 correctedForce = enemyTrans.getOrigin() - playerTrans.getOrigin();
			correctedForce.normalize();

			player.rigidBody->applyCentralImpulse(-correctedForce / 2);

			//_----------------------------------------

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
void AI::attackRanged(MainCharacter &player, Enemy &self, BulletComponents &bulletPhysicsHandler)
{
	if (!rangedAttack && rangedTimer <= 0)
	{
		rangedAttack = true;
		rangedTimer = rangedCd;

		//----------Räkna ut kastet--------------------------------------------------------------
		float forward = self.getAngle(player.getPos()); //x
		float distance = getDistance(player.getPos(), self.getPos()); //så här långt kastar man
		

		float v0 = distance * 10;  //kraften man kastar med
		v0 = sqrtf(v0);

		float v0x = v0 * cos(45);	//kraften i x-led	z-led?
		float v0y = v0 * sin(45);	//kraften i y-led	

		//---------------------------------------------------------------------------------------


		//Spawn a projectile in the enemy's position
		//apply a force (v0) to the projectile towards the player's position

		//the projectile needs its own rigidbody. 
		//Possibly reuse. When crasching, snap back to enemyPos.


		//possible functions---- 
		self.shootProjectile(v0x, v0y, forward);


	}

	if (rangedAttack)
	{
		if (rangedTimer > 0)
			rangedTimer -= timer.getDeltaTime();
		else
		{
			rangedAttack = false;
		}
		//play enemy attack animation here
	}
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

XMMATRIX AI::rotate(XMFLOAT3 playerPos, Enemy &self)
{
	XMMATRIX R;
	float angle = self.getAngle(playerPos);
	R = XMMatrixRotationY(angle);

	return R;
}
