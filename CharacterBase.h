#ifndef CHARACTERBASE_H
#define CHARACTERBASE_H
#include<string>
#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include "Timer.h"
#include "Camera.h"
#include "VertexType.h"

#include "MacroDefinitions.h"

using namespace std;
using namespace DirectX;

class CharacterBase
{
private:
	XMFLOAT3 position;
	int health;
	float movementSpeed;
	int unitID;
	bool alive;


	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

public:
	XMMATRIX tPlayerTranslation;
	BoundingBox bbox;
	
	Timer timer;

	CharacterBase();
	CharacterBase(const bool alive, const int health, const float movementspeed, const int unitID, const XMFLOAT3 position, const XMMATRIX tPlayerTranslation);
	~CharacterBase();

	void releaseAll();

	int getHealth()const;
	void setHealth(const int newHealth);
	float getMovementSpeed()const;
	void setMovementSpeed(const float newMSpeed);
	int getUnitID()const;
	void setUnitID(const int newUnitID);
	bool getAlive()const;
	void setAlive(const bool newAlive);
	XMFLOAT3 getPos()const;
	void setPos(const XMFLOAT3 newPos);

	bool createBuffers(ID3D11Device* &graphicDevice);
	void draw(ID3D11DeviceContext* &graphicDeviceContext);

	void updateWorldMatrix(XMFLOAT3 direction, XMMATRIX rotation);
	void CharacterBase::TransformBoundingBox(BoundingBox bbox, XMMATRIX translation);
	void resetWorldMatrix();

	string toString();
	

};
#endif
