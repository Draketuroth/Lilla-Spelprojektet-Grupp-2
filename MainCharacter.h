#pragma once
#include "CharacterBase.h"
#include "Timer.h"

class MainCharacter: public CharacterBase
{
private:
	Timer timer;
	float deltaTime;

public:
	MainCharacter();
	~MainCharacter();

	void setDeltaTime();

	void update();
	void movement();

	void keyEvent();

	void walk(float d);
	void strafe(float d);
};

