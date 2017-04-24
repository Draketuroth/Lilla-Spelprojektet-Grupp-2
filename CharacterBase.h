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
#include "FBXLoader.h"

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

	XMFLOAT3 boundingBoxExtents;

public:

	XMMATRIX tPlayerTranslation;
	btRigidBody* rigidBody;
	
	Timer timer;

	CharacterBase();
	CharacterBase(const bool alive, const int health, const float movementspeed, const int unitID, const XMFLOAT3 position, const XMMATRIX tPlayerTranslation);
	~CharacterBase();

	void releaseAll();

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

	void CreateBoundingBox(float mass, XMFLOAT3 spawnPos, XMFLOAT3 extents, BulletComponents &bulletPhysicsHandler);
	BoundingBox getBoundingBox();
	XMMATRIX getPlayerTanslationMatrix();

	bool createBuffers(ID3D11Device* &graphicDevice, vector<TriangleVertex>vertices, vector<unsigned int>indices);
	bool createBuffers(ID3D11Device* &graphicDevice, vector<Vertex_Bone>fbxVector, FbxImport &fbxImporter, VS_SKINNED_DATA &skinData);
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
