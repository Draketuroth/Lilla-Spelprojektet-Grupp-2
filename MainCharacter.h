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

	void rotate();

	Camera camera;
	
};

