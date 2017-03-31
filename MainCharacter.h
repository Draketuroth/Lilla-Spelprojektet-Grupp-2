#pragma once
#include "CharacterBase.h"
#include "Timer.h"

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

	void walk(float d);
	void strafe(float d);
};

