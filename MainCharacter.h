#pragma once
#include "CharacterBase.h"

class MainCharacter: public CharacterBase
{
private:


public:
	MainCharacter();
	~MainCharacter();

	void update();
	void movement();

	//BEH�VER DELTA TIME
	//TIMER KLASS? 
	void keyEvent(float deltaTime);

	void walk(float d);
	void strafe(float d);
};

