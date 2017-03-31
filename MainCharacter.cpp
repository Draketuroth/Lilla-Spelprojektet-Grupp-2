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
	//H�R UPPDATERAS MOVEMENT, ANIMATION...
}
void MainCharacter::movement()
{
	//ANV�NDER KEYEVENT F�R ATT R�RA SIG
	keyEvent();

	//BEH�VER MOUSE MOVEMENT F�R ATT ROTERA
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

//�NDRA KARAK�RENS POSITION
void MainCharacter::strafe(float d) {

	/*
	Vi beh�ver s�tta karakt�rens nya position liknande mPosition nedan.
	Karakt�ren kommer inte flytta sig i y-led om den inte faller av plattformen
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
