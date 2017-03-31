#include "MainCharacter.h"



MainCharacter::MainCharacter()
{
	//DEFINERA START HEALTH OSV...
}


MainCharacter::~MainCharacter()
{
}

void MainCharacter::update()
{
	//HÄR UPPDATERAS MOVEMENT. DETTA SKA UPDATERAS VARJE FRAME
}

void MainCharacter::movement()
{
	//ANVÄNDER KEYEVENT FÖR ATT RÖRA SIG
	//BEHÖVER MOUSE MOVEMENT FÖR ATT ROTERA
}

void MainCharacter::keyEvent(float deltaTime)
{
	/*if (GetAsyncKeyState('W') & 0x8000) {

		walk(speed * deltaTime);
	}

	if (GetAsyncKeyState('S') & 0x8000) {

		walk(-speed * deltaTime);
	}

	if (GetAsyncKeyState('A') & 0x8000) {

		strafe(-speed * deltaTime);
	}

	if (GetAsyncKeyState('D') & 0x8000) {

		strafe(speed * deltaTime);
	}*/
}