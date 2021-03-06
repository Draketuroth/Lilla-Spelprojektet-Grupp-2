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

	void initialize(ID3D11Device* &graphicDevice, XMFLOAT3 spawnPosition, BulletComponents &bulletPhysicsHandle, AnimationHandler &animHandler, FileImporter &importer, float radius, float height);
	void update(HWND windowhandle);

	void CharacterMove(HWND windowhandle);
	void CheckInput();

	float characterLookAt(HWND windowHandle);

	void loadVertices(FileImporter &importer, AnimationHandler &fbxImporter, ID3D11Device* &graphicDevice);

	XMMATRIX rotate(HWND windowhandle);
	void meleeAttack(HWND windowHandle, int nrOfEnemies, vector<Enemy*> enemyArray, btDynamicsWorld* bulletDynamicsWorld, float enemyTimePoses[30]);
	void rangeAttack(HWND windowHandle, int nrOfEnemies, vector<Enemy*> enemies, btDynamicsWorld* world, GraphicComponents &gHandler, BufferComponents &bHandler, float enemyTimePoses[30]);

	bool isGrounded();
	void DeathTimer();
	
	ID3D11Buffer* rayBuffer;
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
	bool attackFlag;
	float attackTimer;
	float attackCd;

	bool shooting;
	bool shotFlag;
	float shootTimer;
	float shootCD;

	float deathCountdown;

	float playerAnimTimePos;

	bool renderRay;
	float rayCounter;
	

	sf::Sound attackSound;
	sf::Sound hitSound;
	sf::Sound enemyDeathSound;
	sf::SoundBuffer soundBuffer[5];


	int currentHealth;

	bool GunBuffer(XMFLOAT3 start, XMFLOAT3 end, GraphicComponents &gHandler);


	
	//Don't need this but it contains code.
	//XMVECTOR getPlane();
	//XMFLOAT3 getPointOnRay(XMFLOAT3 ray, float distance);
	//bool IntersectionInRange(XMFLOAT3 MousePos);
	

};

#endif