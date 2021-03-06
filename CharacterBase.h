#ifndef CHARACTERBASE_H
#define CHARACTERBASE_H

#include<string>
#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include "Timer.h"
#include "Camera.h"
#include "VertexType.h"
#include "BasicMath.h"
#include "MacroDefinitions.h"
#include "BufferComponents.h"
#include "GraphicComponents.h"
#include "AnimationHandler.h"
#include "FileImporter.h"


#include <SFML\Audio.hpp>

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

	XMFLOAT3 boundingBoxExtents;
	Mesh_Standard mesh; 


public:
	
	ID3D11Buffer* vertexBuffer;

	XMMATRIX tPlayerTranslation;
	btRigidBody* rigidBody;

	Timer timer;

	CharacterBase();
	CharacterBase(const bool alive, const int health, const float movementspeed, const int unitID, const XMFLOAT3 position, const XMMATRIX tPlayerTranslation);
	~CharacterBase();

	//GET-SET
	int getHealth()const;
	void setHealth(int newHealth);
	float getMovementSpeed()const;
	void setMovementSpeed(const float newMSpeed);
	int getUnitID()const;
	void setUnitID(const int newUnitID);
	bool getAlive()const;
	void setAlive(const bool newAlive);
	XMFLOAT3 getPos()const;
	void setPos(const XMFLOAT3 newPos);

	XMVECTOR forwardVector;

	void CreatePlayerBoundingBox(float mass, XMFLOAT3 spawnPos, float radius, float height, BulletComponents &bulletPhysicsHandler);
	void CreateEnemyBoundingBox(float mass, XMFLOAT3 spawnPos, float radius, float height, BulletComponents &bulletPhysicsHandler, int enemyIndex); 
	BoundingBox getBoundingBox();
	XMMATRIX getPlayerTanslationMatrix();

	btCapsuleShape getBoundingCapsule(); 

	bool createBuffers(ID3D11Device* &graphicDevice, vector<Vertex_Bone>fbxVector, AnimationHandler &animHandler, CHARACTER_SKINNED_DATA &skinData);
	void draw(ID3D11DeviceContext* &graphicDeviceContext, int vertexCount);
	void draw(ID3D11DeviceContext* &graphicDeviceContext);

	//Matrices

	void updateWorldMatrix(XMMATRIX rotation);
	void updateWorldMatrix(XMMATRIX rotation, XMMATRIX scale);
	XMVECTOR getForwardVector();

	void resetWorldMatrix();

	string toString();


};
#endif
