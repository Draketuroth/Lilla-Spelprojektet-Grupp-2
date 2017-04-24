#pragma once
#include "CharacterBase.h"
#include "Enemies.h"


class MainCharacter: public CharacterBase
{
private:


public:
	MainCharacter();
	~MainCharacter();
	
	void initialize(ID3D11Device* &graphicDevice, XMFLOAT3 spawnPosition, BulletComponents &bulletPhysicsHandle, FbxImport &fbxImporter);
	void update(HWND windowhandle);

	void CharacterMove(HWND windowhandle);
	void CheckInput();

	float characterLookAt(HWND windowHandle);

	void loadVertices(FbxImport &fbxImporter, ID3D11Device* &graphicDevice);

	XMMATRIX rotate(HWND windowhandle);
	void meleeAttack(HWND windowHandle, int nrOfEnemies, Enemy enemies[], btDynamicsWorld* bulletDynamicsWorld);
	void rangeAttack(HWND windowHandle, int nrOfEnemies, Enemy enemies[], btDynamicsWorld* world);
	
	//void initiateBB(float mass,BulletComponents &bulletPhysicsHandle);
	int test;

	Camera camera;

	float cameraDistanceY;
	float cameraDistanceZ;
	float playerHeight;

	XMFLOAT3 direction;
	XMFLOAT3 newCameraPos;
	XMVECTOR directionVec;
	XMFLOAT3 floatPos;
	
	vector<Vertex_Bone> fbxVector;
	VS_SKINNED_DATA skinData; // constant buffer struct for inverse bindpose matrices.
	int currentAnimIndex;

	bool attacking;
	float attackTimer;
	float attackCd;

	bool shooting;
	float shootTimer;
	float shootCD;
	//Don't need this but it contains code.
	//XMVECTOR getPlane();
	//XMFLOAT3 getPointOnRay(XMFLOAT3 ray, float distance);
	//bool IntersectionInRange(XMFLOAT3 MousePos);
	
	
	






};

