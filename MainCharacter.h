#pragma once
#include "CharacterBase.h"


class MainCharacter: public CharacterBase
{
private:


public:
	MainCharacter();
	~MainCharacter();

	void initialize(ID3D11Device* &graphicDevice, XMFLOAT3 spawnPosition, BulletComponents &bulletPhysicsHandle);
	void update(HWND windowhandle);

	void CharacterMove(HWND windowhandle);
	void CheckInput();

	float characterLookAt(HWND windowHandle);

	void loadVertices();

	XMMATRIX rotate(HWND windowhandle);
	void meleeAttack(HWND windowHandle);
	void rangeAttack(HWND windowHandle);
	
	void initiateBB(float mass,BulletComponents &bulletPhysicsHandle);


	Camera camera;

	float cameraDistanceY;
	float cameraDistanceZ;
	float playerHeight;

	XMFLOAT3 direction;
	XMFLOAT3 newCameraPos;
	XMVECTOR directionVec;
	XMFLOAT3 floatPos;
	
	vector<TriangleVertex>vertices;
	vector<unsigned int>indices;

	//Don't need this but it contains code.
	//XMVECTOR getPlane();
	//XMFLOAT3 getPointOnRay(XMFLOAT3 ray, float distance);
	//bool IntersectionInRange(XMFLOAT3 MousePos);
	
	
	






};

