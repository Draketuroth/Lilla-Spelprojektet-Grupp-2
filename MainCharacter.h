#pragma once
#include "CharacterBase.h"


class MainCharacter: public CharacterBase
{
private:
	Timer timer;

public:
	MainCharacter();
	~MainCharacter();

	void update();
	void movement();

	XMFLOAT3 getWalkDirection();


	void walkZ(float deltaTime);
	void walkX(float deltaTime);

	void rotate();

	Camera camera;
	
};

