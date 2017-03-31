#include "MainCharacter.h"



MainCharacter::MainCharacter()
	:CharacterBase(true, 10, 3.0f, 1)
{
	this->deltaTime = 0;
}


MainCharacter::~MainCharacter()
{
}

void MainCharacter::setDeltaTime()
{
	this->deltaTime = timer.getDeltaTime();
}

void MainCharacter::update()
{
	//HÄR UPPDATERAS MOVEMENT, ANIMATION...
}

void MainCharacter::movement()
{
	//ANVÄNDER KEYEVENT FÖR ATT RÖRA SIG
	keyEvent();

	//BEHÖVER MOUSE MOVEMENT FÖR ATT ROTERA
}

void MainCharacter::keyEvent()
{

	if (GetAsyncKeyState('W') & 0x8000) {

		walk(getMovementSpeed() * deltaTime);
	}

	if (GetAsyncKeyState('S') & 0x8000) {

		walk(-getMovementSpeed() * deltaTime);
	}

	if (GetAsyncKeyState('A') & 0x8000) {

		strafe(-getMovementSpeed() * deltaTime);
	}

	if (GetAsyncKeyState('D') & 0x8000) {

		strafe(getMovementSpeed() * deltaTime);
	}
}

void MainCharacter::strafe(float d) {

	// mPosition += deltaTime * mRight
	// If creating a vector from a single scalar variable, we use VectorReplicate
	/*XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));*/
}

void MainCharacter::walk(float d) {

	// mPosition += deltaTime * mLook
	// If creating a vector from a single scalar variable, we use VectorReplicate
	/*XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));*/
}
