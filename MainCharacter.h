#pragma once
#include "CharacterBase.h"


class MainCharacter: public CharacterBase
{
private:


public:
	MainCharacter();
	~MainCharacter();

	void update();

	void CharacterMove();
	bool CheckInput(XMFLOAT3 &direction);

	XMMATRIX rotate();



	float cameraDistanceY;
	float cameraDistanceZ;
	float playerHeight;

	XMVECTOR getPlane();


	Camera camera;
	XMVECTOR plane;
};

