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

	void keyEvent();

	void walkZ(float deltaTime);
	void walkX(float deltaTime);

	void rotate();

	void UpdateWorldMatrix();

	Camera camera;
	XMMATRIX tPlayerTranslation;
};

