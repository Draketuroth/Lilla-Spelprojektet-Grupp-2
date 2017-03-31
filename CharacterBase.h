#ifndef CHARACTERBASE_H
#define CHARACTERBASE_H
#include<string>
#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

class CharacterBase
{
public:
	CharacterBase();
	CharacterBase(const bool alive, const float health, const float movementspeed, const int unitID, const float damage);
	~CharacterBase();


	float getHealth()const;
	void setHealth(const float newHealth);
	float getMovementSpeed()const;
	void setMovementSpeed(const float newMSpeed);
	int getUnitID()const;
	void setUnitID(const int newUnitID);
	bool getAlive()const;
	void setAlive(const bool newAlive);
	float getDamage()const;
	void setDamage(const float damage);
	XMFLOAT3 getPos()const;
	void setPos(const XMFLOAT3 newPos);

	string toString();
	
private:

	float health;
	float movementSpeed;
	int unitID;
	bool alive;
	float damage;
	XMFLOAT3 Position;
};




#endif
