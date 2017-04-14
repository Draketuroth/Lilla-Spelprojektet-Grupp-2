#pragma once
#include "CharacterBase.h"


class MainCharacter: public CharacterBase
{
private:


public:
	MainCharacter();
	~MainCharacter();

	void update(HWND windowhandle);

	void CharacterMove(HWND windowhandle);
	bool CheckInput(XMFLOAT3 &direction);

	XMMATRIX rotate(HWND windowhandle);
	void meleeAttack();
	void rangeAttack();
	
	Camera camera;


	float cameraDistanceY;
	float cameraDistanceZ;
	float playerHeight;




	XMFLOAT3 direction;
	XMFLOAT3 newCameraPos;
	XMVECTOR directionVec;
	XMFLOAT3 floatPos;
	

	//Don't need this but it contains code.
	//XMVECTOR getPlane();
	//XMFLOAT3 getPointOnRay(XMFLOAT3 ray, float distance);
	//bool IntersectionInRange(XMFLOAT3 MousePos);

};

