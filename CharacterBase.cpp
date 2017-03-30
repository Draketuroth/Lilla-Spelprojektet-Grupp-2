#include "CharacterBase.h"
#include <sstream>

CharacterBase::CharacterBase()
{
	this->health = 0;
	this->movementSpeed = 0;
	this->unitID = 0;
	this->alive = true;
	this->damage = 0;
}

CharacterBase::CharacterBase(const bool alive, const float health, const float movemenstpeed, const int unitID, const float damage)
{
	this->health = health;
	this->movementSpeed = movemenstpeed;
	this->unitID = unitID;
	this->alive = alive;
	this->damage = damage;
}

CharacterBase::~CharacterBase()
{

}

float CharacterBase::getHealth()const
{
	return this->health;
}

void CharacterBase::setHealth(const float newHealth)
{
	this->health = newHealth;
}

float CharacterBase::getMovementSpeed()const
{
	return this->movementSpeed;
}

void CharacterBase::setMovementSpeed(const float newMSpeed)
{
	this->movementSpeed = newMSpeed;
}

int CharacterBase::getUnitID()const
{
	return this->unitID;
}

void CharacterBase::setUnitID(const int newUnitID)
{
	this->unitID = newUnitID;
}

bool CharacterBase::getAlive()const
{
	return this->alive;
}

void CharacterBase::setAlive(const bool newAlive)
{
	this->alive = newAlive;
}

float CharacterBase::getDamage()const
{
	return this->damage;
}

void CharacterBase::setDamage(const float damage)
{
	this->damage = damage;
}

string CharacterBase::toString()
{
	stringstream toString;

	toString << "Unit ID: " << this->unitID << endl 
		<< "Health: " << this->health << endl 
		<< "Movement Speed: " << this->movementSpeed << endl 
		<< "Alive: " << this->alive << endl;

	return toString.str();
}