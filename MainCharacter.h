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

	Camera camera;

	float cameraDistanceY;
	float cameraDistanceZ;
	float playerHeight;
	
};

