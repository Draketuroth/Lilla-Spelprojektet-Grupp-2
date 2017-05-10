#ifndef MAINCHARACTER_H
#define MAINCHARACTER_H

#include "CharacterBase.h"
#include "Enemies.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"

class MainCharacter: public CharacterBase
{
private:


public:
	MainCharacter();
	~MainCharacter();

	void releaseAll(btDynamicsWorld* bulletDynamicsWorld);

	void initialize(ID3D11Device* &graphicDevice, XMFLOAT3 spawnPosition, BulletComponents &bulletPhysicsHandle, AnimationHandler &animHandler, FileImporter &importer);
	void update(HWND windowhandle);

	void CharacterMove(HWND windowhandle);
	void CheckInput();

	float characterLookAt(HWND windowHandle);

	void loadVertices(FileImporter &importer, AnimationHandler &fbxImporter, ID3D11Device* &graphicDevice);

	XMMATRIX rotate(HWND windowhandle);
	void meleeAttack(HWND windowHandle, int nrOfEnemies, Enemy enemies[], btDynamicsWorld* bulletDynamicsWorld);
	void rangeAttack(HWND windowHandle, int nrOfEnemies, Enemy enemies[], btDynamicsWorld* world, GraphicComponents gHandler, BufferComponents bHandler);
	bool renderRay(GraphicComponents gHandler, BufferComponents bHandler, XMFLOAT3 start, XMFLOAT3 end);
	bool isGrounded();

	Camera camera;

	float cameraDistanceY;
	float cameraDistanceZ;
	float playerHeight;

	XMFLOAT3 direction;
	XMFLOAT3 newCameraPos;
	XMVECTOR directionVec;
	XMFLOAT3 floatPos;
	
	vector<Vertex_Bone> vertices;
	CHARACTER_SKINNED_DATA skinData; // constant buffer struct for inverse bindpose matrices.
	int currentAnimIndex;

	bool attacking;
	float attackTimer;
	float attackCd;

	bool shooting;
	float shootTimer;
	float shootCD;

	sf::Sound attackSound;
	sf::SoundBuffer soundBuffer[2];

	//Don't need this but it contains code.
	//XMVECTOR getPlane();
	//XMFLOAT3 getPointOnRay(XMFLOAT3 ray, float distance);
	//bool IntersectionInRange(XMFLOAT3 MousePos);
	

};

#endif