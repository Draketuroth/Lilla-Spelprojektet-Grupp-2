#include "MainCharacter.h"



MainCharacter::MainCharacter()
	:CharacterBase(true, 10, 3.0f, 1)
{

}


MainCharacter::~MainCharacter()
{
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

		walk(getMovementSpeed() * timer.getDeltaTime());
	}

	if (GetAsyncKeyState('S') & 0x8000) {

		walk(-getMovementSpeed() * timer.getDeltaTime());
	}

	if (GetAsyncKeyState('A') & 0x8000) {

		strafe(-getMovementSpeed() * timer.getDeltaTime());
	}

	if (GetAsyncKeyState('D') & 0x8000) {

		strafe(getMovementSpeed() * timer.getDeltaTime());
	}
}

//ÄNDRA KARAKÄRENS POSITION
void MainCharacter::strafe(float d) {

	/*
	Vi behöver sätta karaktärens nya position liknande mPosition nedan.
	Karaktären kommer inte flytta sig i y-led om den inte faller av plattformen
	Den kommer bara rotera runt y.
	*/

	/*mPosition += deltaTime * mRight
	//If creating a vector from a single scalar variable, we use VectorReplicate
	XMVECTOR s = XMVectorReplicate(d);
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
