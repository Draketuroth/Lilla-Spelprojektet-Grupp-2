#include "AI.h"

AI::AI()
{
	this->attacking = true;
	this->attackTimer = 1.0;
	this->attackCd = 1.5;
	
	this->timer.initialize();
}

AI::~AI()
{
}

//NOTES
//we need the enemy's position to get he distance
void AI::iceAI(MainCharacter &player, Enemy* self, float enemyTimePos)
{
	
	//AI
	float distance = getDistance(player.getPos(), self->getPos());
	if (distance <= 2)
	{
		attackMelee(player, self, enemyTimePos);
	}
	else if (self->getHealth() <= 2 && distance <= 8)
	{
		moveAwayFromPlayer(player.getPos(), self);
	}
	else if (self->getHealth() > 1 && distance > 2)
	{
		moveTowardsPlayer(player.getPos(), self);
	}


	

	timer.updateCurrentTime();

}

void AI::fireAI(MainCharacter &player, Enemy* self, BulletComponents &bulletPhysicsHandler, float enemyTimePos)
{
	float distance = getDistance(player.getPos(), self->getPos());

	if (distance >= 12 && distance <= 16  )
	{
		attackRanged(player, self, bulletPhysicsHandler, enemyTimePos);
	}
	else if (distance <= 10)
	{
		moveAwayFromPlayer(player.getPos(), self);
	}
	else if( distance > 16)
	{
		moveTowardsPlayer(player.getPos(), self);
	}

	btVector3 fireBallPosVec = self->fireBall.projectileRigidBody->getCenterOfMassPosition();
	XMFLOAT3 fireBallPos = { fireBallPosVec.x(), fireBallPosVec.y(), fireBallPosVec.z() };
	if (!self->hasProjectile && getDistance(fireBallPos, self->getPos()) < 5)
	{
		moveAwayFromPlayer(fireBallPos, self);
	}
}

void AI::attackMelee(MainCharacter &player, Enemy* self, float enemyTimePos)
{
	if (!attacking && attackTimer <= 0)
	{

		cout << "ENEMY ATTACK!" << endl;

		enemyTimePos = 0.0f;
		attacking = true;
		self->attackFlag = true;

		attackTimer = attackCd;

		//-----------------Calculate the hit area-------------------------------
		float angle = self->getAngle(player.getPos());

		XMFLOAT3 enemyPos = self->getBoundingBox().Center;
		XMVECTOR enemyDirVec = self->getForwardVector();

		XMVECTOR rotQuat = XMQuaternionRotationAxis(XMVECTOR{ 0, 1, 0 }, angle);
		enemyDirVec = XMVector3Rotate(enemyDirVec, rotQuat);
		enemyDirVec = XMVector3Normalize(enemyDirVec);

		//newC = center for the attack's hitbox
		XMVECTOR newC = XMLoadFloat3(&enemyPos);
		newC += enemyDirVec * 0.4;
		XMFLOAT3 newCenter;
		XMStoreFloat3(&newCenter, newC);

		XMFLOAT3 boxCenter = newCenter;
		XMFLOAT3 boxRange = { 2, 2, 2 };

		BoundingBox meleeBox = BoundingBox(boxCenter, boxRange);

		BoundingBox playerBox = player.getBoundingBox();
		if (playerBox.Intersects(meleeBox) && player.getAlive() == true)
		{
			cout << "PLAYER GOT HIT" << endl;

			player.setHealth(player.getHealth() - 1);

			//Implement knockback
			btTransform playerTrans;
			btTransform enemyTrans;
			self->rigidBody->getMotionState()->getWorldTransform(enemyTrans);
			player.rigidBody->getMotionState()->getWorldTransform(playerTrans);

			btVector3 correctedForce = enemyTrans.getOrigin() - playerTrans.getOrigin();
			correctedForce.normalize();

			player.rigidBody->applyCentralImpulse(-correctedForce / 2);

			//_----------------------------------------

			if (player.getHealth() <= 0 && player.getAlive() == true)
			{
				player.setAlive(false);
				player.playerAnimTimePos = 0.0f;
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
			self->attackFlag = false;
		}
		//play enemy attack animation here
	}	
}

void AI::attackRanged(MainCharacter &player, Enemy* self, BulletComponents &bulletPhysicsHandler, float enemyTimePos)
{
		enemyTimePos = 0.0f;
		self->attackFlag = true;

		//----------Räkna ut kastet--------------------------------------------------------------
		
		XMVECTOR dirVec = { player.getPos().x - self->getPos().x, player.getPos().y - self->getPos().y, player.getPos().z - self->getPos().z };
		XMVector3Normalize(dirVec);

		XMFLOAT3 dir;
		XMStoreFloat3(&dir, dirVec);
		
		float distance = getDistance(player.getPos(), self->getPos()); //så här långt ska man kasta

		float v0 = distance * 10;  //kraften man kastar med
		v0 = sqrtf(v0);

		float v0x = v0 * cos(45);	//kraften i x-led	z-led?
		float v0y = v0 * sin(45);	//kraften i y-led	

		//---------------------------------------------------------------------------------------
	
		self->shootProjectile(v0x, v0y, dir);

}

void AI::moveTowardsPlayer(XMFLOAT3 playerPosition, Enemy *self)
{
	self->moveTowardsPosition(playerPosition);
}

void AI::moveAwayFromPlayer(XMFLOAT3 playerPosition, Enemy *self)
{
	self->avoidPosition(playerPosition);
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

btVector3 AI::collisionEdge(BoundingBox sides[], Enemy* self)
{
	BoundingBox enemy = self->getBoundingBox();
	btVector3 dir = {0, 0, 0};

	if (enemy.Intersects(sides[0]))
	{
		dir = { 0, 0, -2 };
	}
	if (enemy.Intersects(sides[1]))
	{
		dir = { 0, 0, 2 };
	}
	if (enemy.Intersects(sides[2]))
	{
		dir = { 2, 0, 0 };
	}
	if (enemy.Intersects(sides[3]))
	{
		dir = { -2, 0, 0 };
	}

	return dir;
}
